/*
 * Copyright CogniPilot Foundation 2023
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>

#include <zros/private/zros_node_struct.h>
#include <zros/private/zros_pub_struct.h>
#include <zros/private/zros_sub_struct.h>
#include <zros/zros_node.h>
#include <zros/zros_pub.h>
#include <zros/zros_sub.h>

#include <synapse_topic_list.h>

#include "input_mapping.h"

#define MY_STACK_SIZE 3072
#define MY_PRIORITY 4
#define STATE_ANY -1

LOG_MODULE_REGISTER(melm_fsm, CONFIG_CEREBRI_MELM_LOG_LEVEL);

static K_THREAD_STACK_DEFINE(g_my_stack_area, MY_STACK_SIZE);

static void transition(
    void* state,
    bool request,
    const char* request_name,
    int pre,
    uint8_t post,
    char* buf,
    size_t n,
    int* request_seq,
    bool* request_rejected,
    size_t n_guards,
    ...)
{
    uint8_t* state_int = (uint8_t*)state;

    // not requested
    if (!request) {
        return;
    }

    // null transition
    if (*state_int == post) {
        return;
    }

    // LOG_INF("%s pre: %d, post: %d, state: %d", request_name, pre, post, *state_int);
    // pre state required and not matched
    if (pre >= 0 && *state_int != pre) {
        return;
    }

    // new valid request
    (*request_seq)++;

    // check guards
    va_list ap;
    va_start(ap, n_guards);
    for (size_t i = 0; i < n_guards; i++) {
        int guard = va_arg(ap, int);
        const char* guard_txt = va_arg(ap, const char*);
        if (guard) {
            snprintf(buf, n, "deny %s: %s", request_name, guard_txt);
            LOG_WRN("%s", buf);
            *request_rejected = true;
            return;
        }
    }
    va_end(ap);
    snprintf(buf, n, "accept %s", request_name);
    LOG_INF("%s", buf);
    *state_int = post;
    *request_rejected = false;
    return;
}

struct status_input {
    struct input_request req;
    bool safe;
    bool mode_set;
    bool fuel_low;
    bool fuel_critical;
};

struct context {
    struct zros_node node;
    synapse_msgs_Input input;
    synapse_msgs_BatteryState battery_state;
    synapse_msgs_Safety safety;
    synapse_msgs_Status status;
    struct status_input status_input;
    struct zros_sub sub_input, sub_battery_state, sub_safety;
    struct zros_pub pub_status;
    struct k_sem running;
    size_t stack_size;
    k_thread_stack_t* stack_area;
    struct k_thread thread_data;
};

static struct context g_ctx = {
    .node = {},
    .input = synapse_msgs_Input_init_default,
    .battery_state = synapse_msgs_BatteryState_init_default,
    .safety = synapse_msgs_Safety_init_default,
    .status = {
        .has_header = true,
        .header = {
            .frame_id = "base_link",
            .has_stamp = true,
            .seq = 0,
            .stamp = synapse_msgs_Time_init_default,
        },
        .arming = synapse_msgs_Status_Arming_ARMING_DISARMED,
        .fuel = synapse_msgs_Status_Fuel_FUEL_UNKNOWN,
        .fuel_percentage = 0,
        .flag = 0,
        .input_status = synapse_msgs_Status_LinkStatus_STATUS_UNKNOWN,
        .input_source = synapse_msgs_Status_InputSource_INPUT_SOURCE_ETHERNET,
        .topic_source = synapse_msgs_Status_TopicSource_TOPIC_SOURCE_INPUT,
        .mode = synapse_msgs_Status_Mode_MODE_ACTUATORS,
        .power = 0.0,
        .safety = synapse_msgs_Status_Safety_SAFETY_UNKNOWN,
        .status_message = "",
    },
    .sub_battery_state = {},
    .sub_safety = {},
    .pub_status = {},
    .running = Z_SEM_INITIALIZER(g_ctx.running, 1, 1),
    .stack_size = MY_STACK_SIZE,
    .stack_area = g_my_stack_area,
    .thread_data = {},
};

static void melm_fsm_init(struct context* ctx)
{
    LOG_INF("init");
    zros_node_init(&ctx->node, "melm_fsm");
    zros_sub_init(&ctx->sub_battery_state, &ctx->node,
        &topic_battery_state, &ctx->battery_state, 10);
    zros_sub_init(&ctx->sub_safety, &ctx->node, &topic_safety, &ctx->safety, 10);
    zros_sub_init(&ctx->sub_input, &ctx->node, &topic_input, &ctx->input, 10);
    zros_pub_init(&ctx->pub_status, &ctx->node, &topic_status, &ctx->status);
    k_sem_take(&ctx->running, K_FOREVER);
}

static void melm_fsm_fini(struct context* ctx)
{
    LOG_INF("fini");
    zros_sub_fini(&ctx->sub_battery_state);
    zros_sub_fini(&ctx->sub_safety);
    zros_pub_fini(&ctx->pub_status);
    zros_node_fini(&ctx->node);
    k_sem_give(&ctx->running);
}

static void fsm_compute_input(struct status_input* input, const struct context* ctx)
{
    input_request_compute(&input->req, &ctx->input);

    // precompute logical states
    input->mode_set = ctx->status.mode != synapse_msgs_Status_Mode_MODE_UNKNOWN;
#ifdef CONFIG_CEREBRI_SENSE_SAFETY
    input->safe = ctx->safety.status == synapse_msgs_Safety_Status_SAFETY_SAFE || ctx->safety.status == synapse_msgs_Safety_Status_SAFETY_UNKNOWN;
#else
    input->safe = false;
#endif

#ifdef CONFIG_CEREBRI_SENSE_POWER
    input->fuel_low = ctx->battery_state.voltage < CONFIG_CEREBRI_MELM_BATTERY_LOW_MILLIVOLT / 1000.0;
    input->fuel_critical = ctx->battery_state.voltage < CONFIG_CEREBRI_MELM_BATTERY_MIN_MILLIVOLT / 1000.0;
#else
    input->fuel_low = false;
    input->fuel_critical = false;
#endif
}

static void fsm_update(synapse_msgs_Status* status, const struct status_input* input)
{
    // arm transition
    transition(
        &status->arming, // state
        input->req.arm, // request
        "request arm", // label
        synapse_msgs_Status_Arming_ARMING_DISARMED, // pre
        synapse_msgs_Status_Arming_ARMING_ARMED, // post
        status->status_message, sizeof(status->status_message), // status
        &status->request_seq, &status->request_rejected, // request
        // guards
        5,
        status->mode == synapse_msgs_Status_Mode_MODE_UNKNOWN, "mode not set",
        status->mode == synapse_msgs_Status_Mode_MODE_CALIBRATION, "mode calibration",
        input->safe, "safety on",
        input->fuel_critical, "fuel_critical",
        input->fuel_low, "fuel_low");

    // disarm transitions
    transition(
        &status->arming, // state
        input->req.disarm, // request
        "request disarm", // label
        synapse_msgs_Status_Arming_ARMING_ARMED, // pre
        synapse_msgs_Status_Arming_ARMING_DISARMED, // post
        status->status_message, sizeof(status->status_message), // status
        &status->request_seq, &status->request_rejected, // request
        0); // guards

    // disarm transitions
    transition(
        &status->arming, // state
        input->fuel_critical, // request
        "disarm fuel critical", // label
        synapse_msgs_Status_Arming_ARMING_ARMED, // pre
        synapse_msgs_Status_Arming_ARMING_DISARMED, // post
        status->status_message, sizeof(status->status_message), // status
        &status->request_seq, &status->request_rejected, // request
        0); // guards

    transition(
        &status->arming, // state
        input->safe, // request
        "disarm safety engaged", // label
        synapse_msgs_Status_Arming_ARMING_ARMED, // pre
        synapse_msgs_Status_Arming_ARMING_DISARMED, // post
        status->status_message, sizeof(status->status_message), // status
        &status->request_seq, &status->request_rejected, // request
        0); // guards

    // mode transitions
    transition(
        &status->mode, // state
        input->req.mode_actuators, // request
        "request mode actuators", // label
        STATE_ANY, // pre
        synapse_msgs_Status_Mode_MODE_ACTUATORS, // post
        status->status_message, sizeof(status->status_message), // status
        &status->request_seq, &status->request_rejected, // request
        0); // guards

    transition(
        &status->mode, // state
        input->req.mode_velocity, // request
        "request mode velocity", // label
        STATE_ANY, // pre
        synapse_msgs_Status_Mode_MODE_VELOCITY, // post
        status->status_message, sizeof(status->status_message), // status
        &status->request_seq, &status->request_rejected, // request
        0); // guards

    transition(
        &status->mode, // state
        input->req.mode_bezier, // request
        "request mode bezier", // label
        STATE_ANY, // pre
        synapse_msgs_Status_Mode_MODE_BEZIER, // post
        status->status_message, sizeof(status->status_message), // status
        &status->request_seq, &status->request_rejected, // request
        0); // guards

    transition(
        &status->mode, // state
        input->req.mode_calibration, // request
        "request mode calibration", // label
        STATE_ANY, // pre
        synapse_msgs_Status_Mode_MODE_CALIBRATION, // post
        status->status_message, sizeof(status->status_message), // status
        &status->request_seq, &status->request_rejected, // request
        // guards
        1,
        status->arming == synapse_msgs_Status_Arming_ARMING_ARMED, "disarm required");

    // topic source transitions
    transition(
        &status->topic_source, // state
        input->req.topic_source_input, // request
        "request topic source input", // label
        synapse_msgs_Status_TopicSource_TOPIC_SOURCE_ETHERNET, // pre
        synapse_msgs_Status_TopicSource_TOPIC_SOURCE_INPUT, // post
        status->status_message, sizeof(status->status_message), // status
        &status->request_seq, &status->request_rejected, // request
        // guards
        0);

    transition(
        &status->topic_source, // state
        input->req.topic_source_ethernet, // request
        "request topic source ethernet", // label
        synapse_msgs_Status_TopicSource_TOPIC_SOURCE_INPUT, // pre
        synapse_msgs_Status_TopicSource_TOPIC_SOURCE_ETHERNET, // post
        status->status_message, sizeof(status->status_message), // status
        &status->request_seq, &status->request_rejected, // request
        // guards
        0);

    // set timestamp
    stamp_header(&status->header, k_uptime_ticks());
    status->header.seq++;
}

static void status_add_extra_info(synapse_msgs_Status* status,
    struct status_input* input,
    const struct context* ctx)
{
    if (input->req.lights_on) {
        status->flag |= synapse_msgs_Status_Flag_FLAG_LIGHTING;
    } else {
        status->flag &= ~synapse_msgs_Status_Flag_FLAG_LIGHTING;
    }
    if (input->fuel_critical) {
        status->fuel = synapse_msgs_Status_Fuel_FUEL_CRITICAL;
    } else if (input->fuel_low) {
        status->fuel = synapse_msgs_Status_Fuel_FUEL_LOW;
    } else {
        status->fuel = synapse_msgs_Status_Fuel_FUEL_NOMINAL;
    }
    double bat_max = CONFIG_CEREBRI_MELM_BATTERY_MAX_MILLIVOLT / 1000.0;
    double bat_min = CONFIG_CEREBRI_MELM_BATTERY_MIN_MILLIVOLT / 1000.0;
    status->fuel_percentage = 100 * (ctx->battery_state.voltage - bat_min) / (bat_max - bat_min);
    status->power = ctx->battery_state.voltage * ctx->battery_state.current;
#ifdef CONFIG_CEREBRI_SENSE_SAFETY
    if (ctx->safety.status == synapse_msgs_Safety_Status_SAFETY_SAFE) {
        status->safety = synapse_msgs_Status_Safety_SAFETY_SAFE;
    } else if (ctx->safety.status == synapse_msgs_Safety_Status_SAFETY_UNSAFE) {
        status->safety = synapse_msgs_Status_Safety_SAFETY_UNSAFE;
    } else if (ctx->safety.status == synapse_msgs_Safety_Status_SAFETY_UNKNOWN) {
        status->safety = synapse_msgs_Status_Safety_SAFETY_UNKNOWN;
    }
#else
    status->safety = synapse_msgs_Status_Safety_SAFETY_UNSAFE;
#endif
}

static void melm_fsm_run(void* p0, void* p1, void* p2)
{
    struct context* ctx = p0;
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);

    melm_fsm_init(ctx);

    struct k_poll_event events[] = {
        *zros_sub_get_event(&ctx->sub_input),
        *zros_sub_get_event(&ctx->sub_battery_state),
    };

    int64_t input_last_ticks = k_uptime_ticks();
    int64_t input_loss_ticks = 1.0 * CONFIG_SYS_CLOCK_TICKS_PER_SEC;

    while (k_sem_take(&ctx->running, K_NO_WAIT) < 0) {

        // current ticks
        int64_t now_ticks = k_uptime_ticks();

        // wait for input event, publish at 1 Hz regardless
        int rc = 0;
        rc = k_poll(events, ARRAY_SIZE(events), K_MSEC(1000));
        if (rc != 0) {
            LOG_DBG("fsm input/battery polling timeout");
        }

        if (zros_sub_update_available(&ctx->sub_battery_state)) {
            zros_sub_update(&ctx->sub_battery_state);
        }

        if (zros_sub_update_available(&ctx->sub_safety)) {
            zros_sub_update(&ctx->sub_safety);
        }

        if (zros_sub_update_available(&ctx->sub_input)) {
            zros_sub_update(&ctx->sub_input);
            if (ctx->status.input_status == synapse_msgs_Status_LinkStatus_STATUS_LOSS) {
                LOG_DBG("input regained");
            }
            input_last_ticks = now_ticks;
            ctx->status.input_status = synapse_msgs_Status_LinkStatus_STATUS_NOMINAL;
        }

        // check for input loss
        if (ctx->status.input_status != synapse_msgs_Status_LinkStatus_STATUS_LOSS
            && (now_ticks - input_last_ticks) > input_loss_ticks) {
            LOG_DBG("input loss");
            ctx->status.input_status = synapse_msgs_Status_LinkStatus_STATUS_LOSS;
        }

        // perform processing
        fsm_compute_input(&ctx->status_input, ctx);
        fsm_update(&ctx->status, &ctx->status_input);
        status_add_extra_info(&ctx->status, &ctx->status_input, ctx);
        zros_pub_update(&ctx->pub_status);
    }

    melm_fsm_fini(ctx);
}

static int start(struct context* ctx)
{
    k_tid_t tid = k_thread_create(&ctx->thread_data, ctx->stack_area,
        ctx->stack_size,
        melm_fsm_run,
        ctx, NULL, NULL,
        MY_PRIORITY, 0, K_FOREVER);
    k_thread_name_set(tid, "melm_fsm");
    k_thread_start(tid);
    return 0;
}

static int melm_fsm_cmd_handler(const struct shell* sh,
    size_t argc, char** argv, void* data)
{
    ARG_UNUSED(argc);
    struct context* ctx = data;
    if (argc == 1) {
        LOG_ERR("must have one argument");
        return -1;
    }

    if (strcmp(argv[0], "start") == 0) {
        if (k_sem_count_get(&g_ctx.running) == 0) {
            shell_print(sh, "already running");
        } else {
            start(ctx);
        }
    } else if (strcmp(argv[0], "stop") == 0) {
        if (k_sem_count_get(&g_ctx.running) == 0) {
            k_sem_give(&g_ctx.running);
        } else {
            shell_print(sh, "not running");
        }
    } else if (strcmp(argv[0], "status") == 0) {
        shell_print(sh, "running: %d", (int)k_sem_count_get(&g_ctx.running) == 0);
    }
    return 0;
}

SHELL_SUBCMD_DICT_SET_CREATE(sub_melm_fsm, melm_fsm_cmd_handler,
    (start, &g_ctx, "start"),
    (stop, &g_ctx, "stop"),
    (status, &g_ctx, "status"));

SHELL_CMD_REGISTER(melm_fsm, &sub_melm_fsm, "melm fsm commands", NULL);

static int melm_fsm_sys_init(void)
{
    return start(&g_ctx);
};

SYS_INIT(melm_fsm_sys_init, APPLICATION, 1);

// vi: ts=4 sw=4 et

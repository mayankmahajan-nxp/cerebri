/*
 * Copyright CogniPilot Foundation 2023
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <zros/private/zros_node_struct.h>
#include <zros/private/zros_pub_struct.h>
#include <zros/private/zros_sub_struct.h>
#include <zros/zros_node.h>
#include <zros/zros_pub.h>
#include <zros/zros_sub.h>

#include <synapse_topic_list.h>

#include "mixing.h"

#include <zephyr/drivers/pwm.h>

#define MY_STACK_SIZE 1024
#define MY_PRIORITY 4

LOG_MODULE_REGISTER(b3rb_farming, CONFIG_CEREBRI_B3RB_LOG_LEVEL);

typedef struct _context {
    struct zros_node node;
    synapse_msgs_Joy joy_farm;
    struct zros_sub sub_joy_farm;
} context;

static context g_ctx = {
    .node = {},
    .joy_farm = synapse_msgs_Joy_init_default,
    .sub_joy_farm = {},
};

static void init(context* ctx)
{
    zros_node_init(&ctx->node, "b3rb_farming");
    zros_sub_init(&ctx->sub_joy_farm, &ctx->node, &topic_joy_farm, &ctx->joy_farm, 10);
}

typedef struct actuator_pwm_t {
	uint32_t min;
	uint32_t max;
	uint32_t center;
	bool use_nano_seconds;
	const char* alias;
	float slope;
	float intercept;
	uint8_t index;
	struct pwm_dt_spec device;
} actuator_pwm_t;

actuator_pwm_t g_actuator_pwms_farming[] = {
	{
		// .min = CONFIG_CEREBRI_ACTUATE_PWM_PULSE_MIN_3,
		// .max = CONFIG_CEREBRI_ACTUATE_PWM_PULSE_MAX_3,
		// .center = CONFIG_CEREBRI_ACTUATE_PWM_PULSE_CENTER_3,
		// .use_nano_seconds = false,
		// .alias = CONFIG_CEREBRI_ACTUATE_PWM_OUTPUT_3,

		// .index = CONFIG_CEREBRI_ACTUATE_PWM_INDEX_3,
		.device = PWM_DT_SPEC_GET(DT_CHILD(DT_NODELABEL(pwm_shell), aux3)),
	},
};

static void b3rb_farming_entry_point(void* p0, void* p1, void* p2)
{
    LOG_INF("init");
    context* ctx = p0;
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);

    init(ctx);

    struct k_poll_event events[] = {
        *zros_sub_get_event(&ctx->sub_joy_farm),
    };

    int err = 0;

    while (true) {
        int rc = 0;
        rc = k_poll(events, ARRAY_SIZE(events), K_MSEC(1000));
        if (rc != 0) {
            LOG_DBG("manual not receiving joy");
        }

        if (zros_sub_update_available(&ctx->sub_joy_farm)) {
            zros_sub_update(&ctx->sub_joy_farm);
        }

        actuator_pwm_t pwm = g_actuator_pwms_farming[0];
        err = pwm_set_pulse_dt(&pwm.device, PWM_USEC(ctx->joy_farm.axes[0] * 350 + 1500));
    }
}

K_THREAD_DEFINE(b3rb_farming, MY_STACK_SIZE,
    b3rb_farming_entry_point, (void*)&g_ctx, NULL, NULL,
    MY_PRIORITY, 0, 1000);

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
#include <zephyr/drivers/gpio.h>

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

actuator_pwm_t g_actuator_pwms_farming_1[] = {
	{
		.device = PWM_DT_SPEC_GET(DT_CHILD(DT_NODELABEL(pwm_shell), aux3)),
	},
};

actuator_pwm_t g_actuator_pwms_farming_2[] = {
	{
		.device = PWM_DT_SPEC_GET(DT_CHILD(DT_NODELABEL(pwm_shell), aux4)),
	},
};

#define USER_LED2_RED_NODE DT_NODELABEL(user_led2_red)
const struct device *led_dev = DEVICE_DT_GET(DT_GPIO_CTLR(USER_LED2_RED_NODE, gpios));
gpio_pin_t led_pin = DT_GPIO_PIN(USER_LED2_RED_NODE, gpios);
gpio_flags_t led_flags = DT_GPIO_FLAGS(USER_LED2_RED_NODE, gpios);

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

    if (!device_is_ready(led_dev)) {
        printk("LED device not ready\n");
        return;
    }

    int ret = gpio_pin_configure(led_dev, led_pin, GPIO_OUTPUT_ACTIVE | led_flags);
    if (ret != 0) {
        printk("Failed to configure LED pin\n");
    }

    while (true) {
        int rc = 0;
        rc = k_poll(events, ARRAY_SIZE(events), K_MSEC(1000));
        if (rc != 0) {
            LOG_DBG("manual not receiving joy");
        }

        if (zros_sub_update_available(&ctx->sub_joy_farm)) {
            zros_sub_update(&ctx->sub_joy_farm);
        }

        actuator_pwm_t pwm_1 = g_actuator_pwms_farming_1[0];
        actuator_pwm_t pwm_2 = g_actuator_pwms_farming_2[0];
        err = pwm_set_pulse_dt(&pwm_1.device, PWM_USEC(ctx->joy_farm.axes[0] * 350 + 1500));
        err = pwm_set_pulse_dt(&pwm_2.device, PWM_USEC(ctx->joy_farm.axes[1] * 350 + 1500));
        LOG_ERR("manual not receiving joy %f", ctx->joy_farm.axes[0]);
        LOG_ERR("manual not receiving joy %f", ctx->joy_farm.axes[1]);

        gpio_pin_set(led_dev, led_pin, 1);
        k_sleep(K_MSEC(100));
        gpio_pin_set(led_dev, led_pin, 0);
        k_sleep(K_MSEC(1000));
    }
}

K_THREAD_DEFINE(b3rb_farming, MY_STACK_SIZE,
    b3rb_farming_entry_point, (void*)&g_ctx, NULL, NULL,
    MY_PRIORITY, 0, 1000);

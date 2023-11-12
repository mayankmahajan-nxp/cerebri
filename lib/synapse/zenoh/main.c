/*
 * Copyright CogniPilot Foundation 2023
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zenoh-pico.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(synapse_zenoh, CONFIG_CEREBRI_SYNAPSE_ZENOH_LOG_LEVEL);

#define MY_STACK_SIZE 8192
#define MY_PRIORITY 5

#define MODE "client"
#define CONNECT ""

#define KEYEXPR "demo/example/**"

typedef struct _context {
    int counter;
} context;

static context g_ctx = {
    .counter = 0
};

void data_handler(const z_sample_t* sample, void* arg)
{
    z_owned_str_t keystr = z_keyexpr_to_string(sample->keyexpr);
    LOG_WRN(" >> [Subscriber handler] Received ('%s': '%.*s')\n", z_loan(keystr),
        (int)sample->payload.len, sample->payload.start);
    z_drop(z_move(keystr));
}

static void synapse_zenoh_entry_point(context* ctx)
{
    // wait 5 seconds
    k_msleep(5000);

    // Initialize Zenoh Session and other parameters
    z_owned_config_t config = z_config_default();
    zp_config_insert(z_loan(config), Z_CONFIG_MODE_KEY, z_string_make(MODE));
    // zp_config_insert(z_loan(config), Z_CONFIG_CONNECT_KEY, z_string_make(CONNECT));

    // Open Zenoh session
    LOG_WRN("Z_FEATURE_SUBSCRIPTION: %d", Z_FEATURE_SUBSCRIPTION);
    LOG_WRN("Z_FEATURE_PUBLICATION: %d", Z_FEATURE_PUBLICATION);
    LOG_WRN("Z_FEATURE_MULTI_THREAD: %d", Z_FEATURE_MULTI_THREAD);
    LOG_WRN("Z_FEATURE_DYNAMIC_MEMORY_ALLOCATION: %d", Z_FEATURE_DYNAMIC_MEMORY_ALLOCATION);
    LOG_WRN("Opening Zenoh Session...");
    z_owned_session_t s = z_open(z_move(config));
    if (!z_check(s)) {
        LOG_WRN("Unable to open session!\n");
        return;
    }
    LOG_WRN("OK");

    // Start the receive and the session lease loop for zenoh-pico
    /*zp_start_read_task(z_loan(s), NULL);*/
    /*zp_start_lease_task(z_loan(s), NULL);*/

    /*LOG_WRN("Declaring Subscriber on '%s'...", KEYEXPR);*/
    /*z_owned_closure_sample_t callback = z_closure(data_handler);*/
    /*z_owned_subscriber_t sub = z_declare_subscriber(z_loan(s), z_keyexpr(KEYEXPR), z_move(callback), NULL);*/

    /*if (!z_check(sub)) {*/
    /*LOG_WRN("Unable to declare subscriber.");*/
    /*return;*/
    /*}*/

    while (1) {
        k_msleep(5000);
        LOG_WRN("Zenoh loop running...");
    }

    LOG_WRN("Closing Zenoh Session...");
    /*z_undeclare_subscriber(z_move(sub));*/

    // Stop the receive and the session lease loop for zenoh-pico
    /*zp_stop_read_task(z_loan(s));*/
    /*zp_stop_lease_task(z_loan(s));*/

    /*z_close(z_move(s));*/
    LOG_WRN("OK!");
}

K_THREAD_DEFINE(synapse_zenoh, MY_STACK_SIZE, synapse_zenoh_entry_point,
    &g_ctx, NULL, NULL, MY_PRIORITY, 0, 0);

/* vi: ts=4 sw=4 et */

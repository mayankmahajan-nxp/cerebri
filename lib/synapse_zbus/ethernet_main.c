/*
 * Copyright CogniPilot Foundation 2023
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pb_decode.h>
#include <pb_encode.h>

#include <synapse_tinyframe/SynapseTopics.h>
#include <synapse_tinyframe/TinyFrame.h>
#include <synapse_tinyframe/utils.h>

#include <synapse_zbus/channels.h>

#define MY_STACK_SIZE 1024
#define MY_PRIORITY -1

#define TX_BUF_SIZE 1024
#define RX_BUF_SIZE 1024

#define BIND_PORT 4242

static TinyFrame* g_tf = NULL;
static int g_client = 0;

static TF_Result genericListener(TinyFrame* tf, TF_Msg* msg)
{
    dumpFrameInfo(msg);
    return TF_STAY;
}

#define TOPIC_LISTENER(CHANNEL, CLASS)                                         \
    static TF_Result CHANNEL##_Listener(TinyFrame* tf, TF_Msg* frame)          \
    {                                                                          \
        CLASS msg = CLASS##_init_zero;                                         \
        pb_istream_t stream = pb_istream_from_buffer(frame->data, frame->len); \
        int status = pb_decode(&stream, CLASS##_fields, &msg);                 \
        if (status) {                                                          \
            zbus_chan_pub(&chan_##CHANNEL, &msg, K_FOREVER);                   \
        } else {                                                               \
            printf("Decoding failed: %s\n", PB_GET_ERROR(&stream));            \
        }                                                                      \
        return TF_STAY;                                                        \
    }

TOPIC_LISTENER(in_bezier_trajectory, BezierTrajectory);
TOPIC_LISTENER(in_cmd_vel, Twist);
TOPIC_LISTENER(in_joy, Joy);
TOPIC_LISTENER(in_odometry, Odometry);


void listener_synapse_zbus_ethernet_callback(const struct zbus_channel *chan) {
    if (chan == &chan_out_actuators) {
        TF_Msg msg;
        TF_ClearMsg(&msg);
        uint8_t buf[500];
        pb_ostream_t stream = pb_ostream_from_buffer((pu8)buf, sizeof(buf));
        int status = pb_encode(&stream, Actuators_fields, chan->message);
        if (status) {
            msg.type = SYNAPSE_OUT_ACTUATORS_TOPIC;
            msg.data = buf;
            msg.len =  stream.bytes_written;
            if (g_tf != NULL) {
                TF_Send(g_tf, &msg);
            }
        } else {
            printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
        }
    }
}

ZBUS_LISTENER_DEFINE(listener_synapse_zbus_ethernet, listener_synapse_zbus_ethernet_callback);


static void write_ethernet(TinyFrame* tf, const uint8_t* buf, uint32_t len)
{
    int out_len;
    const char* p;
    p = buf;
    do {
        out_len = send(g_client, p, len, 0);
        if (out_len < 0) {
            printf("error: send: %d\n", errno);
            return;
        }
        p += out_len;
        len -= out_len;
    } while (len);
}

static void ethernet_entry_point(void)
{
    static uint8_t rx1_buf[RX_BUF_SIZE];

    int serv;
    struct sockaddr_in bind_addr;
    static int counter;

    serv = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (serv < 0) {
        printf("error: socket: %d\n", errno);
        exit(1);
    }

    bind_addr.sin_family = AF_INET;
    bind_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind_addr.sin_port = htons(BIND_PORT);

    if (bind(serv, (struct sockaddr*)&bind_addr, sizeof(bind_addr)) < 0) {
        printf("error: bind: %d\n", errno);
        exit(1);
    }

    if (listen(serv, 5) < 0) {
        printf("error: listen: %d\n", errno);
        exit(1);
    }

    printf("TCP server waits for a connection on "
           "port %d...\n",
        BIND_PORT);

    // Set up the TinyFrame library
    g_tf = TF_Init(TF_MASTER);
    g_tf->write = write_ethernet;

    TF_AddGenericListener(g_tf, genericListener);
    TF_AddTypeListener(g_tf, SYNAPSE_IN_BEZIER_TRAJECTORY_TOPIC, in_bezier_trajectory_Listener);
    TF_AddTypeListener(g_tf, SYNAPSE_IN_CMD_VEL_TOPIC, in_cmd_vel_Listener);
    TF_AddTypeListener(g_tf, SYNAPSE_IN_JOY_TOPIC, in_joy_Listener);
    TF_AddTypeListener(g_tf, SYNAPSE_IN_ODOMETRY_TOPIC, in_odometry_Listener);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        char addr_str[32];
        g_client = accept(serv, (struct sockaddr*)&client_addr,
            &client_addr_len);

        if (g_client < 0) {
            printf("error: accept: %d\n", errno);
            continue;
        } else {
            printf("connected\n");
        }

        inet_ntop(client_addr.sin_family, &client_addr.sin_addr,
            addr_str, sizeof(addr_str));
        printf("Connection #%d from %s\n", counter++, addr_str);

        while (1) {
            // send cmd vel topic
            /*
            {
                TF_Msg frame;
                Twist msg = Twist_init_zero;
                msg.has_linear = true;
                msg.linear.x = 1;
                msg.linear.y = 2;
                msg.linear.z = 3;
                msg.has_angular = true;
                msg.angular.x = 4;
                msg.angular.y = 5;
                msg.angular.z = 6;
                pb_ostream_t tx_stream = pb_ostream_from_buffer(tx1_buf, Twist_size);
                pb_encode(&tx_stream, Twist_fields, &msg);

                TF_ClearMsg(&frame);
                frame.type = SYNAPSE_OUT_CMD_VEL_TOPIC;
                frame.len = tx_stream.bytes_written;
                frame.data = (pu8)tx1_buf;
                TF_Send(tf, &frame);
            }
            */

            // receive messages
            {
                int len = recv(g_client, rx1_buf, sizeof(rx1_buf), 0);
                TF_Accept(g_tf, rx1_buf, len);
                // printf("len: %d\n", len);
            }

            // should move tf tick to a clock thread
            TF_Tick(g_tf);
        }
    }
}

K_THREAD_DEFINE(synapse_zbus_ethernet, MY_STACK_SIZE, ethernet_entry_point,
    NULL, NULL, NULL, MY_PRIORITY, 0, 0);

/* vi: ts=4 sw=4 et */

/*
 * Copyright CogniPilot Foundation 2023
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mixing.h"

void melm_set_actuators(synapse_msgs_Actuators* msg, double turn_angle, double omega_fwd, bool armed)
{
    msg->has_header = true;
    stamp_header(&msg->header, k_uptime_ticks());
    msg->header.seq++;
    strncpy(msg->header.frame_id, "odom", sizeof(msg->header.frame_id) - 1);

    if (!armed) {
        // stop if not armed
        turn_angle = 0;
        omega_fwd = 0;
    }

    msg->position_count = 1;
    msg->position[0] = turn_angle;

    msg->velocity_count = 1;
    msg->velocity[0] = omega_fwd;

    msg->normalized_count = 1;
    msg->normalized[0] = armed ? 1 : -1;
}

/* vi: ts=4 sw=4 et */

/*
 * Copyright CogniPilot Foundation 2023
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CEREBRI_MELM_MIXING_H
#define CEREBRI_MELM_MIXING_H

#include <synapse_topic_list.h>

void melm_set_actuators(synapse_msgs_Actuators* msg, double turn_angle, double omega_fwd, bool armed);

#endif // CEREBRI_MELM_MIXING_H
/* vi: ts=4 sw=4 et */

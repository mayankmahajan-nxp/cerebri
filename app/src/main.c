/*
 * Copyright (c) 2023 CogniPilot Foundation
 * SPDX-License-Identifier: Apache-2.0
 */

#if defined(CONFIG_ARCH_POSIX)
#include <signal.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>

#include "app_version.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

static volatile int keepRunning = 1;

#if defined(CONFIG_ARCH_POSIX)
void intHandler(int dummy)
{
    (void)dummy;
    keepRunning = 0;
    printf("sigint caught\n");
    exit(0);
}
#endif

void main(void)
{
    const char * banner =
    "                            \033[0m\033[38;5;252m              ▄▄▄▄▄▄▄▄\n"
    "\033[2;34m         ▄▄▄▄▄ \033[2;33m▄▄▄▄▄\033[0m\033[38;5;252m                    ▀▀▀▀▀▀▀▀▀\n"
    "\033[2;34m     ▄███████▀\033[2;33m▄██████▄\033[0m\033[38;5;252m   ▀█████████████████████▀\n"
    "\033[2;34m  ▄██████████ \033[2;33m████████\033[31m ▄\033[0m\033[38;5;249m   ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄\n"
    "\033[2;34m ███████████▀ \033[2;33m███████▀\033[31m ██\033[0m\033[38;5;249m   ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀\n"
    "\033[2;34m█████████▀   \033[2;33m▀▀▀▀▀▀▀▀\033[31m ████\033[0m\033[38;5;246m   ▀███████████▀\n"
    "\033[2;34m▀█████▀ \033[2;32m▄▄███████████▄\033[31m ████\033[0m\033[38;5;243m   ▄▄▄▄▄▄▄▄▄\n"
    "\033[2;34m  ▀▀▀ \033[2;32m███████████████▀\033[31m ████\033[0m\033[38;5;243m   ▀▀▀▀▀▀▀▀\n"
    "       \033[2;32m▀▀█████▀▀▀▀▀▀\033[31m  ▀▀▀▀\033[0m\033[38;5;240m   ▄█████▀\n"
    "              \033[2;90m ████████▀    ▄▄▄\n"
    "              \033[2;90m ▀███▀       ▀▀▀\n"
    "              \033[2;90m  ▀▀      \033[0m\n"
    "╔═══╗╔═══╗╔═══╗╔═╗ ╔╗╔══╗╔═══╗╔══╗╔╗   ╔═══╗╔════╗\n"
    "║╔═╗║║╔═╗║║╔═╗║║║║ ║║╚╣╠╝║╔═╗║╚╣╠╝║║   ║╔═╗║║╔╗╔╗║\n"
    "║║ ╚╝║║ ║║║║ ╚╝║║╚╗║║ ║║ ║║ ║║ ║║ ║║   ║║ ║║╚╝║║╚╝\n"
    "║║   ║║ ║║║║╔═╗║╔╗╚╝║ ║║ ║╚═╝║ ║║ ║║   ║║ ║║  ║║  \n"
    "║║ ╔╗║║ ║║║║╚╗║║║╚╗║║ ║║ ║╔══╝ ║║ ║║ ╔╗║║ ║║  ║║  \n"
    "║╚═╝║║╚═╝║║╚═╝║║║ ║║║╔╣╠╗║║   ╔╣╠╗║╚═╝║║╚═╝║ ╔╝╚╗ \n"
    "╚═══╝╚═══╝╚═══╝╚╝ ╚═╝╚══╝╚╝   ╚══╝╚═══╝╚═══╝ ╚══╝ \n\033[31m"
    "       ┏━━━┓┏━━━┓┏━━━┓┏━━━┓┏━━┓ ┏━━━┓┏━━┓\n"
    "       ┃┏━┓┃┃┏━━┛┃┏━┓┃┃┏━━┛┃┏┓┃ ┃┏━┓┃┗┫┣┛\n"
    "       ┃┃ ┗┛┃┗━┓ ┃┗━┛┃┃┗━┓ ┃┗┛┗┓┃┗━┛┃ ┃┃ \n"
    "       ┃┃ ┏┓┃┏━┛ ┃┏┓┏┛┃┏━┛ ┃┏━┓┃┃┏┓┏┛ ┃┃ \n"
    "       ┃┗━┛┃┃┗━━┓┃┃┃┗┓┃┗━━┓┃┗━┛┃┃┃┃┗┓┏┫┣┓\n"
    "       ┗━━━┛┗━━━┛┗┛┗━┛┗━━━┛┗━━━┛┗┛┗━┛┗━━┛\n\033[0m";
    printk("Cerebri %s\n", APP_VERSION_STR);
    printf("%s\n", banner);

#if defined(CONFIG_ARCH_POSIX)
    signal(SIGINT, intHandler);
#endif
    while (keepRunning) {
        k_msleep(10000);
        //int64_t uptime = k_uptime_get() / 1e3;
#if defined(CONFIG_ARCH_POSIX)
        //printf("\nuptime: %ld sec\n", uptime);
#else
        //printf("\nuptime: %lld sec\n", uptime);
#endif
    }
}

// vi: ts=4 sw=4 et

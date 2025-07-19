/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <app_version.h>

/*
 * Copyright (c) 2017 Linaro Limited
 * Copyright (c) 2018 Intel Corporation
 * Copyright (c) 2024 TOKITA Hiroshi
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <string.h>

#define LOG_LEVEL 4
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main);

#include "uart_cmd.h"
#include "led_rgb_strip.h"
#include "message_hmi_main.h"
#include "message_main_ledrgb.h"

// static uint16_t stomp_cnt = 0;
static bool stomp_signal = false;

void handle_hmi_msg(void)
{
    static struct hmi_msg_t hmi_msg = {};
    if (recv_message_hmi_to_main(&hmi_msg) != 0) {
        return;
    }

    // struct main_to_bubble_msg_t main_to_bubble = {0};
    if (hmi_msg.type == HMI_CMD_STOMP_INC) {
        printk("HMI Command to Main Thread: Stomp Increment: Stomps - %u\n", stomp_signal);
        stomp_signal = true;
        // main_to_bubble.type = MAIN_CMD_BUBBLE_ON;
    } else if (hmi_msg.type == HMI_CMD_ON_SIGNAL) {
        printk("HMI Command to Main Thread: On Signal\n");
        // main_to_bubble.type = MAIN_CMD_BUBBLE_OFF;
    } else if (hmi_msg.type == HMI_CMD_OFF_SIGNAL) {
        printk("HMI Command to Main Thread: Off Signal\n");
        // main_to_bubble.type = MAIN_CMD_BUBBLE_OFF;
    } else if (hmi_msg.type == HMI_CMD_RST_STOMP) {
        stomp_signal = false;
        printk("HMI Command to Main Thread: Reset Stomp: Stomps - %u\n", stomp_signal);
        // main_to_bubble.type = MAIN_CMD_BUBBLE_OFF;
    } else {
        return;
    }
}

void send_msg_main_led_rgb(void)
{
    // only send message if stomp effect is received
    if (stomp_signal == false) {
        return;
    }

    struct main_to_ledrgb_msg_t msg = {
        .type = MAIN_CMD_LEDRGB_STOMP,
    };

    if (send_message_main_to_ledrgb(&msg) != 0) {
        printk("Failed to send message to LED RGB thread\n");
    } else {
        stomp_signal = false; // Reset stomp signal after sending
    }
}

int main(void)
{
    printk("Zephyr Example Application %s\n", APP_VERSION_STRING);

    uart_thread_start();
    led_rgb_thread_start();

    while (1) {
        handle_hmi_msg();

        // Send message to the LED RGB Thread
        send_msg_main_led_rgb();

        printk("Main Thread Running\n");
        k_msleep(1000);
    }

    return 0;
}

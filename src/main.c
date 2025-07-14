/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <app_version.h>


// #include <zephyr/kernel.h>
// #include <zephyr/drivers/gpio.h>

// #include "message_main_bubble.h"

// /* PWM device for servo control */
// #define BUBBLE_THREAD_STACK_SIZE 1024
// #define BUBBLE_THREAD_PRIORITY 7

// #if !DT_NODE_HAS_STATUS(DT_ALIAS(led0), okay)
// #error "LED0 GPIO node is not ready"
// #endif

// static const struct gpio_dt_spec bubble_io = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

// // Thread initialization
// K_THREAD_STACK_DEFINE(bubble_thread_stack, BUBBLE_THREAD_STACK_SIZE);
// struct k_thread bubble_thread_data;
// k_tid_t bubble_thread_id;

// // -----------------------------------
// // Static functions
// // -----------------------------------
// static void init_io(void)
// {
//     if (!device_is_ready(bubble_io.port)) {
//         printk("Error: IO device %s is not ready\n", bubble_io.port->name);
//         return;
//     }

//     int ret = gpio_pin_configure_dt(&bubble_io, GPIO_OUTPUT_ACTIVE);
//     if (ret < 0) {
//         printk("Error %d: failed to configure LED pin\n", ret);
//     }

//     gpio_pin_set_dt(&bubble_io, 0);
// }

// void handle_main_msg(void)
// {
//     static struct main_to_bubble_msg_t msg = {};
//     if (recv_message_main_to_bubble(&msg) != 0) {
//         return;
//     }

//     switch (msg.type) {
//         case MAIN_CMD_BUBBLE_ON:
//             printk("BUBBLE: On\n");
//             gpio_pin_set_dt(&bubble_io, 1);
//             break;
//         case MAIN_CMD_BUBBLE_OFF:
//             printk("BUBBLE: Off\n");
//             gpio_pin_set_dt(&bubble_io, 0);
//             break;
//         default:
//             printk("Unknown HMI Command: %d\n", msg.type);
//             break;
//     }
// }

// #include "uart_cmd.h"
// #include "bubble_control.h"
// #include "iot_thread.h"

// #include "message_hmi_main.h"
// #include "message_main_bubble.h"
// #include "message_iot_main.h"

// void handle_hmi_msg(void)
// {
//     static struct hmi_msg_t hmi_msg = {};
//     if (recv_message_hmi_to_main(&hmi_msg) != 0) {
//         return;
//     }

//     switch (hmi_msg.module) {
//         case MOD_MAIN:
//             //printk("HMI Command to Main Thread\n");
//             break;
//         case MOD_BUBBLE:
//             //printk("HMI Command to Bubble Thread\n");
//             struct main_to_bubble_msg_t main_to_bubble = {0};
//             if (hmi_msg.type == HMI_CMD_BUBBLE_ON) {
//                 main_to_bubble.type = MAIN_CMD_BUBBLE_ON;
//             } else if (hmi_msg.type == HMI_CMD_BUBBLE_OFF) {
//                 main_to_bubble.type = MAIN_CMD_BUBBLE_OFF;
//             } else {
//                 return;
//             }
//             send_message_main_to_bubble(&main_to_bubble);
//             //TODO: Handle return from message sent
//             break;
//         default:
//             printk("Command not recognized from HMI: %02X", hmi_msg.type);
//             break;
//     }
// }

// void handle_iot_msg(void)
// {
//     static struct iot_to_main_msg_t iot_msg = {};
//     if (recv_message_iot_to_main(&iot_msg) != 0) {
//         return;
//     }

//     struct main_to_bubble_msg_t main_to_bubble = {0};
//     switch (iot_msg.type) {
//         case IOT_CMD_BUBBLE_ON:
//             main_to_bubble.type = MAIN_CMD_BUBBLE_ON;
//             //printk("HMI Command to Main Thread\n");
//             break;
//         case IOT_CMD_BUBBLE_OFF:
//             main_to_bubble.type = MAIN_CMD_BUBBLE_OFF;
//             break;
//         default:
//             printk("Command not recognized from IOT: %02X", iot_msg.type);
//             return;
//     }
//     send_message_main_to_bubble(&main_to_bubble);
// }

int main(void)
{
    printk("Zephyr Example Application %s\n", APP_VERSION_STRING);

    // bubble_thread_start();
    // //uart_thread_start();
    // iot_thread_start();

    while (1) {
        // Handle communication with all threads
        //handle_hmi_msg();
        // handle_iot_msg();
        // Read data from HMI Thread
        // Read data from Wifi/MQTT
        // Write data to Servo Thread
        // Read data from servo thread
        k_msleep(500);  // Delay to avoid CPU usage
        printk("AIAIAIAI Bixin\n");
    }

    return 0;
}

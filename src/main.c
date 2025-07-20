/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <app_version.h>

#include <errno.h>
#include <string.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>

#include <zephyr/logging/log.h>
#include <zephyr/drivers/adc.h>

#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

#include "uart_cmd.h"
#include "led_rgb_strip.h"
#include "message_hmi_main.h"
#include "message_main_ledrgb.h"

// LOG Configuration
#define LOG_LEVEL 4
LOG_MODULE_REGISTER(main);


// ADC Configuration
#if !DT_NODE_EXISTS(DT_PATH(zephyr_user)) || \
	!DT_NODE_HAS_PROP(DT_PATH(zephyr_user), io_channels)
#error "No suitable devicetree overlay specified"
#endif

#define DT_SPEC_AND_COMMA(node_id, prop, idx) \
	ADC_DT_SPEC_GET_BY_IDX(node_id, idx),

/* Data of ADC io-channels specified in devicetree. */
static const struct adc_dt_spec adc_channels[] = {
	DT_FOREACH_PROP_ELEM(DT_PATH(zephyr_user), io_channels,
			     DT_SPEC_AND_COMMA)
};

// Global variables
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
    // if (stomp_signal == false) {
    //     return;
    // }

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

    // ADC INIT
    int err;
	uint32_t count = 0;
	uint16_t buf;
	struct adc_sequence sequence = {
		.buffer = &buf,
		/* buffer size in bytes, not number of samples */
		.buffer_size = sizeof(buf),
	};

	/* Configure channels individually prior to sampling. */
	for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) {
		if (!adc_is_ready_dt(&adc_channels[i])) {
			printk("ADC controller device %s not ready\n", adc_channels[i].dev->name);
			return 0;
		}

		err = adc_channel_setup_dt(&adc_channels[i]);
		if (err < 0) {
			printk("Could not setup channel #%d (%d)\n", i, err);
			return 0;
		}
	}

    while (1) {
        handle_hmi_msg();

        // Send message to the LED RGB Thread
        if (stomp_signal) {
            send_msg_main_led_rgb();
        }

        // ADC Reading
		printk("ADC reading[%u]:\n", count++);
		for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) {
			int32_t val_mv;

			printk("- %s, channel %d: ",
			       adc_channels[i].dev->name,
			       adc_channels[i].channel_id);

			(void)adc_sequence_init_dt(&adc_channels[i], &sequence);

			err = adc_read_dt(&adc_channels[i], &sequence);
			if (err < 0) {
				printk("Could not read (%d)\n", err);
				continue;
			}

			/*
			 * If using differential mode, the 16 bit value
			 * in the ADC sample buffer should be a signed 2's
			 * complement value.
			 */
			if (adc_channels[i].channel_cfg.differential) {
				val_mv = (int32_t)((int16_t)buf);
			} else {
				val_mv = (int32_t)buf;
			}
			printk("%"PRId32, val_mv);
			err = adc_raw_to_millivolts_dt(&adc_channels[i],
						       &val_mv);
			/* conversion to mV may not be supported, skip if not */
			if (err < 0) {
				printk(" (value in mV not available)\n");
			} else {
				printk(" = %"PRId32" mV\n", val_mv);
			}
		}

		k_sleep(K_MSEC(1000));



        //printk("Main Thread Running\n");
        //k_msleep(50);
    }

    return 0;
}

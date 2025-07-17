/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/led_strip.h>
// #include <zephyr/device.h>
// #include <zephyr/drivers/spi.h>
// #include <zephyr/sys/util.h>

#include <errno.h>
#include <string.h>

#define LOG_LEVEL 4
LOG_MODULE_REGISTER(led_rgb_strip);

// #include <zephyr/kernel.h>

// #include "uart_cmd.h"
// #include "message_hmi_main.h"


#define STRIP_NODE		DT_ALIAS(led_strip)

#if DT_NODE_HAS_PROP(DT_ALIAS(led_strip), chain_length)
#define STRIP_NUM_PIXELS	DT_PROP(DT_ALIAS(led_strip), chain_length)
#else
#error Unable to determine length of LED strip
#endif

#define DELAY_TIME K_MSEC(CONFIG_SAMPLE_LED_UPDATE_DELAY)

#define RGB(_r, _g, _b) { .r = (_r), .g = (_g), .b = (_b) }

static const struct led_rgb colors[] = {
	RGB(CONFIG_SAMPLE_LED_BRIGHTNESS, 0x00, 0x00), /* red */
	RGB(0x00, CONFIG_SAMPLE_LED_BRIGHTNESS, 0x00), /* green */
	RGB(0x00, 0x00, CONFIG_SAMPLE_LED_BRIGHTNESS), /* blue */
};

static struct led_rgb pixels[STRIP_NUM_PIXELS];
static const struct device *const strip = DEVICE_DT_GET(STRIP_NODE);

 // Thread initialization
#define LED_RGB_STRIP_THREAD_STACK_SIZE 2048 // Increase from 1024 to 2048
#define LED_RGB_STRIP_THREAD_PRIORITY 7

K_THREAD_STACK_DEFINE(led_rgb_strip_thread_stack, LED_RGB_STRIP_THREAD_STACK_SIZE);
struct k_thread led_rgb_strip_thread_data;
k_tid_t led_rgb_strip_thread_id;

// -----------------------------------
// Static functions
// -----------------------------------

void handle_main_msg(void)
{
}

static void led_rgb_strip_thread_entry(void *p1, void *p2, void *p3)
{
	if (device_is_ready(strip)) {
		LOG_INF("Found LED strip device %s", strip->name);
	} else {
		LOG_ERR("LED strip device %s is not ready", strip->name);
		return;
	}

	LOG_INF("Displaying pattern on strip with %d pixels", STRIP_NUM_PIXELS);
	size_t color = 0;
	int rc;


    while (1) {
        handle_main_msg();
        printk("LED RGB strip thread running\n");

        // // Simple test - set all LEDs to the current color
        for (size_t i = 0; i < STRIP_NUM_PIXELS; i++) {
            pixels[i] = colors[color];
        }

        rc = led_strip_update_rgb(strip, pixels, STRIP_NUM_PIXELS);
        if (rc) {
            LOG_ERR("couldn't update strip: %d", rc);
        } else {
            LOG_INF("Updated all %d LEDs with color %zu: R=%02x G=%02x B=%02x",
                    STRIP_NUM_PIXELS,
                    color,
                    colors[color].r,
                    colors[color].g,
                    colors[color].b);
        }

        k_sleep(K_MSEC(50));  // Wait 1 second between color changes
        color = (color + 1) % ARRAY_SIZE(colors);

        k_msleep(1000);
    }

    return;
}

// -----------------------------------
// Public functions
// -----------------------------------
void led_rgb_thread_start(void)
{
    led_rgb_strip_thread_id = k_thread_create(&led_rgb_strip_thread_data, led_rgb_strip_thread_stack,
                                    K_THREAD_STACK_SIZEOF(led_rgb_strip_thread_stack),
                                    led_rgb_strip_thread_entry, NULL, NULL, NULL,
                                    LED_RGB_STRIP_THREAD_PRIORITY, 0, K_NO_WAIT);
    printk("LED RGB STRIP thread created\n");
}

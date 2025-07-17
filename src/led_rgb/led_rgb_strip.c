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
	RGB(0x00, 0x00, 0x00), /* NO COLLOR */
	RGB(CONFIG_SAMPLE_LED_BRIGHTNESS, 0x00, 0x00), /* red */
	RGB(0x00, 0xff, 0x00), /* green */
	RGB(0x00, 0x00, 0xff), /* blue */
	RGB(0x00, 0x00, 0x7f), /* blue */
	RGB(0x00, 0x00, 0x3f), /* blue */
};

static struct led_rgb pixels[STRIP_NUM_PIXELS];
static const struct device *const strip = DEVICE_DT_GET(STRIP_NODE);

 // Thread initialization
#define LED_RGB_STRIP_THREAD_STACK_SIZE 2048 // Increase from 1024 to 2048
#define LED_RGB_STRIP_THREAD_PRIORITY 7

#define STRIP_MIDDLE_NUM_PIXELS (STRIP_NUM_PIXELS / 2)

K_THREAD_STACK_DEFINE(led_rgb_strip_thread_stack, LED_RGB_STRIP_THREAD_STACK_SIZE);
struct k_thread led_rgb_strip_thread_data;
k_tid_t led_rgb_strip_thread_id;

// -----------------------------------
// Static functions
// -----------------------------------

void handle_main_msg(void)
{
}

static void led_rgb_wave_effect_from_middle(void)
{
    static int wave_effect_initialized = 2;
    static size_t wave_position_left, wave_position_right=0;
    int rc;
    static int aux=0;
    static int cnt = 0;
    if (!wave_effect_initialized) {
        // Initialize wave effect parameters here if needed
        if (cnt == 5) {
            return;
        }
        wave_effect_initialized = true;
        wave_position_left = STRIP_MIDDLE_NUM_PIXELS + 2; // Start from the middle of the strip
        wave_position_right = STRIP_MIDDLE_NUM_PIXELS; // Start from the middle of the strip
        cnt++;
    }

    for (size_t i = 0; i < STRIP_MIDDLE_NUM_PIXELS; i++) {
        if (i == wave_position_left) {
            pixels[i].r = 0x00; // Set the color for the wave effect
            pixels[i].g = 0x00;
            pixels[i].b = 0x01;
        } else if (i == (wave_position_left - 1)) {
            pixels[i].r = 0x00; // Set the color for the wave effect
            pixels[i].g = 0x00;
            pixels[i].b = 0x0f; // Slightly dimmer blue
        } else if (i == (wave_position_left - 2)) {
            pixels[i].r = 0x00; // Set the color for the wave effect
            pixels[i].g = 0x00;
            pixels[i].b = 0xff; // Even dimmer blue
        } else {
            pixels[i].r = 0x00; // Set to no color for other pixels
            pixels[i].g = 0x00;
            pixels[i].b = 0x00;
        }
    }

    rc = led_strip_update_rgb(strip, pixels, STRIP_NUM_PIXELS);
    if (rc) {
        LOG_ERR("couldn't update strip: %d", rc);
    }

    if (wave_position_left <= 0) {
        // Reset wave effect parameters if needed
        wave_effect_initialized = false;
        return;
    }

    wave_position_left--;
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

    while (1) {
        handle_main_msg();
        led_rgb_wave_effect_from_middle(); // Call the wave effect function

        k_usleep(12600);
        //k_msleep(100);
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

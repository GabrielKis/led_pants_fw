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

// #include <zephyr/kernel.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/sys/util.h>

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

////////////////////////

// /* PWM device for servo control */
// #define BUBBLE_THREAD_STACK_SIZE 1024
// #define BUBBLE_THREAD_PRIORITY 7

#if !DT_NODE_HAS_STATUS(DT_ALIAS(led0), okay)
#error "LED0 GPIO node is not ready"
#endif

static const struct gpio_dt_spec bubble_io = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

// Thread initialization
// K_THREAD_STACK_DEFINE(bubble_thread_stack, BUBBLE_THREAD_STACK_SIZE);
// struct k_thread bubble_thread_data;
// k_tid_t bubble_thread_id;

// -----------------------------------
// Static functions
// -----------------------------------
static void init_io(void)
{
    if (!device_is_ready(bubble_io.port)) {
        printk("Error: IO device %s is not ready\n", bubble_io.port->name);
        return;
    }

    int ret = gpio_pin_configure_dt(&bubble_io, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        printk("Error %d: failed to configure LED pin\n", ret);
    }

    gpio_pin_set_dt(&bubble_io, 1);
}


int main(void)
{
    printk("Zephyr Example Application %s\n", APP_VERSION_STRING);
    init_io();

	size_t color = 0;
	int rc;

	if (device_is_ready(strip)) {
		LOG_INF("Found LED strip device %s", strip->name);
	} else {
		LOG_ERR("LED strip device %s is not ready", strip->name);
		return 0;
	}

	LOG_INF("Displaying pattern on strip with %d pixels", STRIP_NUM_PIXELS);

    while (1) {
        // Simple test - set all LEDs to the current color
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

        k_sleep(K_MSEC(200));  // Wait 1 second between color changes
        color = (color + 1) % ARRAY_SIZE(colors);
    }

    return 0;
}

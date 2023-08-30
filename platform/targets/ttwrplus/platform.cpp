/***************************************************************************
 *   Copyright (C) 2023 by Federico Amedeo Izzo IU2NUO,                    *
 *                         Niccolò Izzo IU2KIN                             *
 *                         Silvano Seva IU2KWO                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>   *
 ***************************************************************************/

#include <interfaces/platform.h>
#include <interfaces/delays.h>
#include <hwconfig.h>
#include <ui.h>
#include "pmu.h"

#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/led_strip.h>

/*
 * Get buttons devicetree object to access PTT button
 */
#define BUTTON_PTT_NODE	DT_NODELABEL(button_ptt)
static const struct gpio_dt_spec button_ptt = GPIO_DT_SPEC_GET_OR(BUTTON_PTT_NODE, gpios, {0});

/*
 * Rotary encoder is read using hardware pulse counter configured as quadrature decoder
 */
static const struct device *const qdec_dev = DEVICE_DT_GET(DT_ALIAS(qdec0));

/*
 * Initialize WS2812C RGB LED
 */
#define LED_NODE    DT_ALIAS(led0)
#define RGB(_r, _g, _b) { .r = (_r), .g = (_g), .b = (_b) }
struct led_rgb led_color = RGB(0x00, 0x00, 0x00);
static const struct device *const led_dev = DEVICE_DT_GET(LED_NODE);

static hwInfo_t hwInfo =
{
    .name        = "ttwrplus",

    .hw_version  = 0,

    ._unused     = 0,
    .uhf_band    = 0,
    .vhf_band    = 0,

    .uhf_maxFreq = 480,
    .uhf_minFreq = 400,

    .vhf_maxFreq = 174,
    .vhf_minFreq = 134,
};

void platform_init()
{
    int ret = 0;
    // Initialize GPIO for PTT and rotary encoder
    if (!gpio_is_ready_dt(&button_ptt)) {
        printk("Error: button device %s is not ready\n",
               button_ptt.port->name);
    }
    ret = gpio_pin_configure_dt(&button_ptt, GPIO_INPUT);
    if (ret != 0) {
        printk("Error %d: failed to configure %s pin %d\n",
               ret, button_ptt.port->name, button_ptt.pin);
    }
    // Rotary encoder is read using hardware pulse counter
    if (!device_is_ready(qdec_dev)) {
        printk("Qdec device is not ready\n");
    }
    // Initialize PMU
    pmu_init();
    // Initialize LED to off state
    if (!device_is_ready(led_dev)) {
        printk("LED device %s is not ready", led_dev->name);
    }
    ret = led_strip_update_rgb(led_dev, &led_color, 1);
    if (ret) {
        printk("couldn't update strip: %d", ret);
    }
}

void platform_terminate()
{
}

uint16_t platform_getVbat()
{
    return pmu_getVbat();
}

uint8_t platform_getMicLevel()
{
    return 0;
}

uint8_t platform_getVolumeLevel()
{
    return 0;
}

uint8_t platform_getChSelector()
{
    struct sensor_value val;
    int rc = sensor_sample_fetch(qdec_dev);
    if (rc != 0) {
        printk("Failed to fetch sample (%d)\n", rc);
        return 0;
    }
    rc = sensor_channel_get(qdec_dev, SENSOR_CHAN_ROTATION, &val);
    if (rc != 0) {
        printk("Failed to get data (%d)\n", rc);
        return 0;
    }
    return (val.val1 * (int32_t) 255) / 360 % 255;
}

bool platform_getPttStatus()
{
    return gpio_pin_get_dt(&button_ptt);
}

bool platform_pwrButtonStatus()
{
    return true;
}

void platform_ledOn(led_t led)
{
    int ret = 0;
    switch(led)
    {
        case GREEN:
            led_color.g = 0xff;
            break;
        case RED:
            led_color.r = 0xff;
            break;
        default:
            break;
    }
    ret = led_strip_update_rgb(led_dev, &led_color, 1);
    if (ret) {
        printk("couldn't update strip: %d", ret);
    }
}

void platform_ledOff(led_t led)
{
    int ret = 0;
    switch(led)
    {
        case GREEN:
            led_color.g = 0x00;
            break;
        case RED:
            led_color.r = 0x00;
            break;
        default:
            break;
    }
    ret = led_strip_update_rgb(led_dev, &led_color, 1);
    if (ret) {
        printk("couldn't update strip: %d", ret);
    }
}

void platform_beepStart(uint16_t freq)
{
    (void) freq;
}

void platform_beepStop()
{
    ;
}

hwInfo_t *platform_getHwInfo()
{
    return &hwInfo;
}


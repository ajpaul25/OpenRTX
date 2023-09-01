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
#include <hwconfig.h>

#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>

/*
 * Get buttons devicetree object to access PTT button
 */
#define BUTTON_PTT_NODE	DT_NODELABEL(button_ptt)
static const struct gpio_dt_spec button_ptt = GPIO_DT_SPEC_GET_OR(BUTTON_PTT_NODE, gpios, {0});

/*
 * Rotary encoder is read using hardware pulse counter configured as quadrature decoder
 */
static const struct device *const qdec_dev = DEVICE_DT_GET(DT_ALIAS(qdec0));

static const hwInfo_t hwInfo =
{
    .uhf_maxFreq = 430,
    .uhf_minFreq = 440,
    .uhf_band    = 1,
    .name        = "ttwrplus"
};


void platform_init()
{
    int ret = 0;
    // Setup GPIO for PTT and rotary encoder
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
        return 0;
    }
}

void platform_terminate()
{
}

uint16_t platform_getVbat()
{
    return 0;
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
    (void) led;
}

void platform_ledOff(led_t led)
{
    (void) led;
}

void platform_beepStart(uint16_t freq)
{
    (void) freq;
}

void platform_beepStop()
{
    ;
}

const hwInfo_t *platform_getHwInfo()
{
    return &hwInfo;
}


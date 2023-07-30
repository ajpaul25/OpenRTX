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

/*
 * Get buttons devicetree object to access PTT and rotary encoder
 */
#define BUTTON_PTT_NODE	DT_NODELABEL(button_ptt)
static const struct gpio_dt_spec button_ptt = GPIO_DT_SPEC_GET_OR(BUTTON_PTT_NODE, gpios, {0});
#define ENCODER_A_NODE	DT_NODELABEL(encoder_a)
static const struct gpio_dt_spec encoder_a = GPIO_DT_SPEC_GET_OR(ENCODER_A_NODE, gpios, {0});
#define ENCODER_B_NODE	DT_NODELABEL(encoder_b)
static const struct gpio_dt_spec encoder_b = GPIO_DT_SPEC_GET_OR(ENCODER_B_NODE, gpios, {0});

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
	if (!gpio_is_ready_dt(&encoder_a)) {
		printk("Error: button device %s is not ready\n",
		       encoder_a.port->name);
	}
	ret = gpio_pin_configure_dt(&encoder_a, GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n",
		       ret, encoder_a.port->name, encoder_a.pin);
	}
	if (!gpio_is_ready_dt(&encoder_b)) {
		printk("Error: button device %s is not ready\n",
		       encoder_b.port->name);
	}
	ret = gpio_pin_configure_dt(&encoder_b, GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n",
		       ret, encoder_b.port->name, encoder_b.pin);
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

int8_t platform_getChSelector()
{
    // Rotary encoder on T-TWR Plus has 2 bits and uses gray encoding
    static const uint8_t grayEncoding[] = { 0, 1, 3, 2 };
		int pos = gpio_pin_get_dt(&encoder_a) | gpio_pin_get_dt(&encoder_b) << 1;
    return grayEncoding[pos];
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
}

const hwInfo_t *platform_getHwInfo()
{
    return &hwInfo;
}


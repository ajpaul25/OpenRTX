/***************************************************************************
 *   Copyright (C) 2020 - 2023 by Federico Amedeo Izzo IU2NUO,             *
 *                                Niccolò Izzo IU2KIN                      *
 *                                Frederik Saraci IU2NRO                   *
 *                                Silvano Seva IU2KWO                      *
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

#include <stdio.h>
#include <stdint.h>
#include <peripherals/gpio.h>
#include <interfaces/delays.h>
#include <interfaces/keyboard.h>
#include <interfaces/platform.h>
#include "hwconfig.h"

#include <zephyr/drivers/gpio.h>
#include <zephyr/input/input.h>

static uint8_t old_pos = 0;
keyboard_t keys = 0;

static const struct device *const buttons_dev = DEVICE_DT_GET(DT_NODELABEL(buttons));

static void gpio_keys_cb_handler(struct input_event *evt)
{
    // Map betweek Zephyr keys and OpenRTX keys
    switch (evt->code) {
        case INPUT_KEY_VOLUMEDOWN:
            keys = (evt->value) ? keys | KEY_DOWN : keys & ~KEY_DOWN;
            break;
        case INPUT_BTN_START:
            keys = (evt->value) ? keys | KEY_ENTER : keys & ~KEY_ENTER;
        case INPUT_BTN_SELECT:
            keys = (evt->value) ? keys | KEY_ESC : keys & ~KEY_ESC;
            break;
    }
}
INPUT_LISTENER_CB_DEFINE(buttons_dev, gpio_keys_cb_handler);

void kbd_init()
{
    /* Initialise old position */
    old_pos = platform_getChSelector();
}

void kbd_terminate()
{
  ;
}

keyboard_t kbd_getKeys()
{
    /* Read rotary encoder to send KNOB_LEFT and KNOB_RIGHT events */
    int16_t new_pos = platform_getChSelector();
    keys &= ~KNOB_LEFT;
    keys &= ~KNOB_RIGHT;
    if (old_pos != new_pos)
    {
        // Handle overflow case
        if ((new_pos == 0 && old_pos == 255) || new_pos > old_pos)
            keys |= KNOB_LEFT;
        else
            keys |= KNOB_RIGHT;
        old_pos = new_pos;
    }
    return keys;
}

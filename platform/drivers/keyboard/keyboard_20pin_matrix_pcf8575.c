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

#include <peripherals/gpio.h>
#include <interfaces/delays.h>
#include <interfaces/keyboard.h>
#include "hwconfig.h"
#include <pcf8575.h>

void kbd_init()
{
    return;
    pcf8575_gpio_setMode(KB_ROW0, INPUT_PULL_UP);
    pcf8575_gpio_setMode(KB_ROW1, INPUT_PULL_UP);
    pcf8575_gpio_setMode(KB_ROW2, INPUT_PULL_UP);
    pcf8575_gpio_setMode(KB_ROW3, INPUT_PULL_UP);
    pcf8575_gpio_setMode(KB_ROW4, INPUT_PULL_UP);
    pcf8575_gpio_setMode(KB_COL0, OUTPUT);
    pcf8575_gpio_setMode(KB_COL1, OUTPUT);
    pcf8575_gpio_setMode(KB_COL2, OUTPUT);
    pcf8575_gpio_setMode(KB_COL3, OUTPUT);
}

keyboard_t kbd_getKeys()
{
    keyboard_t keys = 0;

    pcf8575_gpio_clearPin(KB_COL0);
    delayUs(1);
    if(pcf8575_gpio_readPin(KB_ROW0) == 0) keys |= KEY_F1;
    if(pcf8575_gpio_readPin(KB_ROW1) == 0) keys |= KEY_1;
    if(pcf8575_gpio_readPin(KB_ROW2) == 0) keys |= KEY_4;
    if(pcf8575_gpio_readPin(KB_ROW3) == 0) keys |= KEY_7;
    if(pcf8575_gpio_readPin(KB_ROW4) == 0) keys |= KEY_LEFT;
    pcf8575_gpio_setPin(KB_COL0);

    pcf8575_gpio_clearPin(KB_COL1);
    delayUs(1);
    if(pcf8575_gpio_readPin(KB_ROW0) == 0) keys |= KEY_MONI;
    if(pcf8575_gpio_readPin(KB_ROW1) == 0) keys |= KEY_2;
    if(pcf8575_gpio_readPin(KB_ROW2) == 0) keys |= KEY_5;
    if(pcf8575_gpio_readPin(KB_ROW3) == 0) keys |= KEY_8;
    if(pcf8575_gpio_readPin(KB_ROW4) == 0) keys |= KEY_0;
    pcf8575_gpio_setPin(KB_COL1);

    pcf8575_gpio_clearPin(KB_COL2);
    delayUs(1);
    if(pcf8575_gpio_readPin(KB_ROW0) == 0) keys |= KEY_HASH;
    if(pcf8575_gpio_readPin(KB_ROW1) == 0) keys |= KEY_3;
    if(pcf8575_gpio_readPin(KB_ROW2) == 0) keys |= KEY_6;
    if(pcf8575_gpio_readPin(KB_ROW3) == 0) keys |= KEY_9;
    if(pcf8575_gpio_readPin(KB_ROW4) == 0) keys |= KEY_RIGHT;
    pcf8575_gpio_setPin(KB_COL2);

    pcf8575_gpio_clearPin(KB_COL3);
    delayUs(1);
    if(pcf8575_gpio_readPin(KB_ROW0) == 0) keys |= KEY_STAR;
    if(pcf8575_gpio_readPin(KB_ROW1) == 0) keys |= KEY_UP;
    if(pcf8575_gpio_readPin(KB_ROW2) == 0) keys |= KEY_DOWN;
    if(pcf8575_gpio_readPin(KB_ROW3) == 0) keys |= KEY_ESC;
    if(pcf8575_gpio_readPin(KB_ROW4) == 0) keys |= KEY_ENTER;
    pcf8575_gpio_setPin(KB_COL3);

    return keys;
}


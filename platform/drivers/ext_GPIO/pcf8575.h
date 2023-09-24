/***************************************************************************
 *   Copyright (C) 2020 - 2023 by Silvano Seva IU2KWO                      *
 *                            and Niccolò Izzo IU2KIN                      *
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

#ifndef PCF8575_H
#define PCF8575_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Driver for multiple external GPIO devices
 *
 */

/**
 * Initialise
 */
void pcf8575_init();

/**
 * Terminate
 */
void pcf8575_terminate();

/*
void pcf8575_gpio_setMode(void *port, uint8_t pin, enum Mode mode);

void pcf8575_setAlternateFunction(void *port, uint8_t pin, uint8_t afNum);

void pcf8575_setOutputSpeed(void *port, uint8_t pin, enum Speed spd);

void pcf8575_setPin(void *port, uint8_t pin);

void pcf8575_clearPin(void *port, uint8_t pin);

void pcf8575_togglePin(void *port, uint8_t pin);

uint8_t pcf8575_readPin(const void *port, uint8_t pin);
*/

#ifdef __cplusplus
}
#endif

#endif /* PCF8575_H */

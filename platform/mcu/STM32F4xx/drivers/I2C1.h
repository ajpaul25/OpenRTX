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

#ifndef I2C1_H
#define I2C1_H

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
void i2c1_init();

/**
 * Terminate
 */
void i2c1_terminate();

bool i2c1_send(unsigned char address,
            uint8_t data, int len, bool sendStop);

bool i2c1_recv(unsigned char address, void *data, int len);

#ifdef __cplusplus
}
#endif

#endif /* I2C1_H */

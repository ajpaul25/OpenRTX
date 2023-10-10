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

#include <hwconfig.h>
#include <peripherals/gpio.h>
#include <drivers/stm32f2_f4_i2c.h>
#include "I2C1.h"

using namespace miosix;
I2C1Driver *i2c;

void i2c1_init()
{
    i2c = &I2C1Driver::instance();
    gpio_setMode(I2C1_SDA, ALTERNATE_OD);
    gpio_setMode(I2C1_SCL, ALTERNATE_OD);
    gpio_setAlternateFunction(I2C1_SDA, 4);
    gpio_setAlternateFunction(I2C1_SCL, 4);
}

void i2c1_terminate()
{
    gpio_setMode(I2C1_SDA, OUTPUT);
    gpio_setMode(I2C1_SCL, OUTPUT);
}

bool i2c1_send(unsigned char address,
            uint8_t data, int len, bool sendStop = true) {
  return i2c->send(address, (void *)&data, 1, sendStop);
}

bool i2c1_recv(unsigned char address, void *data, int len) {
  return i2c->recv(address, data, len);
}

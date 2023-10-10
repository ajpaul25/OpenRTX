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
#include <drivers/stm32f2_f4_i2c.h>
#include "pcf8575.h"

using namespace miosix;

I2C1Driver *i2c;
uint16_t pinmode; //bit=1 signifies input
uint16_t outputValue; //bit=0 signifies active
uint16_t inputValue; //bit=0 signifies active


void pcf8575_init()
{
    pinmode = 0xFFFF; //this is the default state of the device after powerup
    outputValue = pinmode;
    // pins must be high to be used as input.  this implies active-low
    //initialize i2c
    i2c = &I2C1Driver::instance();
    //set all pins to pinmode
    i2c->send(0x20,(void*)&pinmode,0x10); //under normal circumstances, this is unnecessary, but this enforces a known state
}

void pcf8575_terminate()
{
    pinmode = outputValue = 0xFFFF;
    i2c->send(0x20,(void*)&pinmode,0x10);
}

void pcf8575_gpio_setMode(uint8_t pin, enum Mode mode)
{
    if(mode == OUTPUT) {
      pinmode &= ~pin;
    } else if (mode == INPUT) {
      pinmode |= pin;
    }
    i2c->send(0x20,(void*)&pinmode,0x10);
}

void pcf8575_gpio_setPin(uint8_t pin)
{
    if(((pinmode >> pin) & 0x1) == 0x0){
      outputValue |= 0x1 << pin;
      i2c->send(0x20,(void*)&outputValue,0x10);
    }
}

void pcf8575_gpio_clearPin(uint8_t pin)
{
    if(((pinmode >> pin) & 0x1) == 0x0){
      outputValue &= ~(0x1 << pin);
      i2c->send(0x20,(void*)&outputValue,0x10);
    }
}

void pcf8575_gpio_togglePin(uint8_t pin)
{
    if(((pinmode >> pin) & 0x1) == 0x0){
      outputValue ^= 0x1 << pin;
      i2c->send(0x20,(void*)&outputValue,0x10);
    }
}

uint8_t pcf8575_gpio_readPin(uint8_t pin)
{
    if(((pinmode >> pin) & 0x1) == 0x1){
      i2c->recv(0x20,(void*)&inputValue,0x10);
      return (inputValue >> pin) & 0x1;
    }
    return false;
}

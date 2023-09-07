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

#include <peripherals/gpio.h>
#include <interfaces/delays.h>
#include <hwconfig.h>
#include <math.h>
#include "MC145158.h"

#define REF_CLK 5000000.0F  /* Reference clock: 5MHz                 */
#define STEP 12500

void MC145158_init()
{
    gpio_setMode(PLL_CLK, OUTPUT);
    gpio_setMode(PLL_DAT, OUTPUT);
    gpio_setMode(PLL_LE,  OUTPUT);
    gpio_setPin(PLL_LE);
    gpio_setMode(PLL_LD, INPUT);
}

void MC145158_terminate()
{
    gpio_setMode(PLL_CLK, INPUT);
    gpio_setMode(PLL_DAT, INPUT);
    gpio_setMode(PLL_LE,  INPUT);
}

void _pllSendByte(uint8_t b)
{
    for( int i=0; i<8; i++) 
    {
        if( (b<<i) & 0x80 )
            gpio_setPin(PLL_DAT);
        else
            gpio_clearPin(PLL_DAT);

        delayUs(2);
        gpio_setPin(PLL_CLK);
        delayUs(2);
        gpio_clearPin(PLL_CLK);
    }
    delayUs(2);
    gpio_setPin(PLL_DAT);
    delayUs(2);
}

void _pllLatch()
{
    gpio_setPin(PLL_LE);
    delayUs(2);
    gpio_clearPin(PLL_LE);
    delayUs(2);
}

void MC145158_setFrequency(float freq, uint8_t clkDiv)
{
    unsigned long int n, r, nt;
    uint8_t a, b[5];
    nt = freq / STEP;
    n = nt / clkDiv;
    a = n - (n * clkDiv);
    r = REF_CLK / STEP;
    b[0] = (n >> 8) & 0xFF;
    b[1] = n & 0xFF;
    b[2] = a << 1;
    b[3] = (r >> 7) & 0xFF;
    b[4] = (r << 1) + 1;

    // MC145158 synthesizer
    // 5 bytes sent one byte at a time
    // take note of the bit shifting
    // 0000 00nn | nnnn nnnn | aaaa aaa0 | rrrr rrrr | rrrr rrr1
    // 3 bytes, latch, 2 bytes, latch

    _pllSendByte(b[0]);
    _pllSendByte(b[1]);
    _pllSendByte(b[2]);
    _pllLatch();
    _pllSendByte(b[3]);
    _pllSendByte(b[4]);
    _pllLatch();
    gpio_clearPin(PLL_DAT);
}

bool MC145158_isPllLocked()
{
    return (gpio_readPin(PLL_LD) == 1) ? true : false;
}


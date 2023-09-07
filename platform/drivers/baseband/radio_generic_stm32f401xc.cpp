/***************************************************************************
 *   Copyright (C) 2021 - 2023 by Federico Amedeo Izzo IU2NUO,             *
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

#include <interfaces/radio.h>
#include <peripherals/gpio.h>
#include <hwconfig.h>
#include "MC145158.h"

#define IF_FREQ 45100000 // Intermediate frequency 45.1MHz
#define PLL_DIVISOR 127

enum  opstatus      radioStatus;   // Current operating status
const rtxStatus_t  *config;              // Pointer to data structure with radio configuration



void radio_init(const rtxStatus_t *rtxState)
{
    config = rtxState;

    radioStatus = OFF;
    gpio_setMode(PTT_SW, INPUT);
    gpio_setMode(PTT_OUT, OUTPUT);
    gpio_setMode(SPK_MUTE, OUTPUT);
    gpio_setMode(MIC_MUTE, OUTPUT);

    gpio_clearPin(PTT_OUT);
    gpio_clearPin(SPK_MUTE);
    gpio_clearPin(MIC_MUTE);

    radio_disableAfOutput();

    MC145158_init();
}

void radio_terminate()
{
    MC145158_terminate();
    radioStatus = OFF;
}

void radio_tuneVcxo(const int16_t vhfOffset, const int16_t uhfOffset)
{
    (void) vhfOffset;
    (void) uhfOffset;
}

void radio_setOpmode(const enum opmode mode)
{
    switch(mode)
    {
        case OPMODE_FM:
	    gpio_setPin(MIC_MUTE);
	    // set MCU->Speaker output to input/hi-z?
            break;

        case OPMODE_DMR:
	    gpio_clearPin(MIC_MUTE);
	    radio_disableAfOutput();
            break;

        case OPMODE_M17:
	    gpio_clearPin(MIC_MUTE);
	    radio_disableAfOutput();
            break;

        default:
            break;
    }
}

bool radio_checkRxDigitalSquelch()
{
    return false;
}

void radio_enableAfOutput()
{
    gpio_setPin(SPK_MUTE);
}

void radio_disableAfOutput()
{
    gpio_clearPin(SPK_MUTE);
}

void radio_enableRx()
{
    // Set PLL frequency
    MC145158_setFrequency(config->rxFrequency-IF_FREQ, PLL_DIVISOR);

    radioStatus = RX;
    gpio_clearPin(PTT_OUT);
}

void radio_enableTx()
{
    if(config->txDisable == 1) return;

    // Set PLL frequency
    MC145158_setFrequency(config->txFrequency, PLL_DIVISOR);

    radioStatus = TX;
    gpio_setPin(PTT_OUT);
}

void radio_disableRtx()
{
    radioStatus = OFF;
    gpio_clearPin(PTT_OUT);
}

void radio_updateConfiguration()
{

}

float radio_getRssi()
{
    return 0.0f;
}

enum opstatus radio_getStatus()
{
    return radioStatus;
}

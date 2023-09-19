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

#include <interfaces/delays.h>
#include <interfaces/audio.h>
#include <interfaces/radio.h>
#include <peripherals/gpio.h>
#include <hwconfig.h>
#include "toneGenerator_generic.h"
#include "stm32_pwm.h"
#include "stm32_adc.h"

#define PATH(x,y) ((x << 4) | y)

static const uint8_t pathCompatibilityMatrix[9][9] =
{
    // MIC-SPK MIC-RTX MIC-MCU RTX-SPK RTX-RTX RTX-MCU MCU-SPK MCU-RTX MCU-MCU
    {    0   ,   0   ,   0   ,   1   ,   0   ,   1   ,   1   ,   0   ,   1   },  // MIC-RTX
    {    0   ,   0   ,   0   ,   0   ,   1   ,   1   ,   0   ,   0   ,   1   },  // MIC-SPK
    {    0   ,   0   ,   0   ,   1   ,   1   ,   0   ,   1   ,   1   ,   0   },  // MIC-MCU
    {    0   ,   1   ,   1   ,   0   ,   0   ,   0   ,   0   ,   1   ,   1   },  // RTX-SPK
    {    1   ,   0   ,   1   ,   0   ,   0   ,   0   ,   1   ,   0   ,   1   },  // RTX-RTX
    {    1   ,   1   ,   0   ,   0   ,   0   ,   0   ,   1   ,   1   ,   0   },  // RTX-MCU
    {    0   ,   1   ,   1   ,   0   ,   1   ,   1   ,   0   ,   0   ,   0   },  // MCU-SPK
    {    0   ,   0   ,   1   ,   1   ,   0   ,   1   ,   0   ,   0   ,   0   },  // MCU-RTX
    {    1   ,   1   ,   0   ,   1   ,   1   ,   0   ,   0   ,   0   ,   0   }   // MCU-MCU
};


static void stm32pwm_startCbk()
{
    toneGen_lockBeep();
    TIM3->CCER |= TIM_CCER_CC3E;
    TIM3->CR1  |= TIM_CR1_CEN;
}

static void stm32pwm_stopCbk()
{
    TIM3->CCER &= ~TIM_CCER_CC3E;
    toneGen_unlockBeep();
}

static const struct PwmChannelCfg stm32pwm_cfg =
{
    &(TIM3->CCR3),
    stm32pwm_startCbk,
    stm32pwm_stopCbk
};

const struct audioDevice outputDevices[] =
{
    {NULL,                    NULL,          0, SINK_MCU},
    {&stm32_pwm_audio_driver, &stm32pwm_cfg, 0, SINK_SPK},
    {&stm32_pwm_audio_driver, &stm32pwm_cfg, 0, SINK_RTX},
};

const struct audioDevice inputDevices[] =
{
    {NULL,                    0,                 0,              SOURCE_MCU},
    {&stm32_adc_audio_driver, (const void *) 6, STM32_ADC_ADC1, SOURCE_RTX},
    {&stm32_adc_audio_driver, (const void *) 7, STM32_ADC_ADC1, SOURCE_MIC},
};

void audio_init()
{
    gpio_setMode(AUDIO_MIC, INPUT_ANALOG);
    gpio_setMode(SPK_MUTE, OUTPUT);
    gpio_setMode(BASEBAND_RX, INPUT_ANALOG);
    gpio_setMode(MIC_MUTE, OUTPUT);
    gpio_setMode(AUDIO_SPK, ALTERNATE);
    gpio_setMode(BASEBAND_TX, ALTERNATE);
    gpio_setAlternateFunction(BASEBAND_TX, 2);
    gpio_setAlternateFunction(AUDIO_SPK,  2);

    gpio_clearPin(SPK_MUTE);          // Speaker muted
    gpio_clearPin(MIC_MUTE);          // Mic muted

    stm32pwm_init();
    stm32adc_init(STM32_ADC_ADC1);
}

void audio_terminate()
{
    gpio_clearPin(SPK_MUTE);          // Speaker muted
    gpio_clearPin(MIC_MUTE);          // Mic muted

    stm32pwm_terminate();
    stm32adc_terminate();
}

void audio_connect(const enum AudioSource source, const enum AudioSink sink)
{
    uint32_t path = PATH(source, sink);

    switch(path)
    {
        case PATH(SOURCE_MIC, SINK_SPK):
            break;

        case PATH(SOURCE_MIC, SINK_RTX):
            gpio_setPin(MIC_MUTE);
            break;

        case PATH(SOURCE_MIC, SINK_MCU):
            break;

        case PATH(SOURCE_RTX, SINK_SPK):
            radio_enableAfOutput();
            break;

        case PATH(SOURCE_MCU, SINK_SPK):
            break;

        case PATH(SOURCE_MCU, SINK_RTX):
            gpio_setMode(AUDIO_SPK, ALTERNATE);
            break;

        default:
            break;
    }
}

void audio_disconnect(const enum AudioSource source, const enum AudioSink sink)
{
    uint32_t path = PATH(source, sink);

    switch(path)
    {
        case PATH(SOURCE_MIC, SINK_SPK):
            break;

        case PATH(SOURCE_MIC, SINK_RTX):
            gpio_clearPin(MIC_MUTE);
            break;

        case PATH(SOURCE_MIC, SINK_MCU):
            break;

        case PATH(SOURCE_RTX, SINK_SPK):
            radio_disableAfOutput();
            break;

        case PATH(SOURCE_MCU, SINK_SPK):
            break;

        case PATH(SOURCE_MCU, SINK_RTX):
            gpio_setMode(AUDIO_SPK, INPUT);  // Set output to Hi-Z
            break;

        default:
            break;
    }
}

bool audio_checkPathCompatibility(const enum AudioSource p1Source,
                                  const enum AudioSink   p1Sink,
                                  const enum AudioSource p2Source,
                                  const enum AudioSink   p2Sink)

{
    uint8_t p1Index = (p1Source * 3) + p1Sink;
    uint8_t p2Index = (p2Source * 3) + p2Sink;

    return pathCompatibilityMatrix[p1Index][p2Index] == 1;
}

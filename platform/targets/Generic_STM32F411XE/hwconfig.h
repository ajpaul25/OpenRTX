/***************************************************************************
 *   Copyright (C) 2021 - 2023 by Federico Amedeo Izzo IU2NUO,             *
 *                                Niccolò Izzo IU2KIN,                     *
 *                                Frederik Saraci IU2NRO,                  *
 *                                Silvano Seva IU2KWO                      *
 *                                Mathis Schmieder DB9MAT                  *
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

#ifndef HWCONFIG_H
#define HWCONFIG_H

#include <stm32f4xx.h>

/* Screen dimensions */
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

/* Screen has adjustable contrast */
#define SCREEN_CONTRAST
#define DEFAULT_CONTRAST 91

/* Screen pixel format */
#define PIX_FMT_BW

/* Device has no battery */
#define BAT_NONE

/* Signalling LEDs */
//#define PTT_LED     GPIOB,
//#define SYNC_LED    GPIOB,
//#define ERR_LED     GPIOB,

/* Display */
#define LCD_RST     GPIOA,15
#define LCD_RS      GPIOA,4
//#define LCD_CS      GPIOA,5
#define SPI2_CLK    GPIOB,13
//#define SPI2_SDO    GPIOB,14     // UNUSED
#define SPI2_SDI    GPIOB,15

/* Keyboard */
#define KB_ROW0     GPIOB,0
#define KB_ROW1     GPIOB,1
#define KB_ROW2     GPIOB,2
#define KB_ROW3     GPIOB,3
#define KB_ROW4     GPIOB,4
#define KB_COL0     GPIOB,8
#define KB_COL1     GPIOB,9
#define KB_COL2     GPIOB,10
#define KB_COL3     GPIOB,12


#define PTT_SW      GPIOA,0	//STM32F401 PA0, BOOT0 
#define PTT_OUT     GPIOC,13	//STM32F401 Green LED

/* Audio */
#define AUDIO_MIC   GPIOA,7	//STM32F401 ADC7
#define AUDIO_SPK   GPIOA,1	//STM32F401 PWM T2_CH2
#define BASEBAND_RX GPIOA,6	//STM32F401 ADC6
#define BASEBAND_TX GPIOA,5	//STM32F401 PWM T2_CH1
#define SPK_MUTE    GPIOB,14
#define MIC_MUTE    GPIOB,5

/* PLL */
#define PLL_LE      GPIOA,8
#define PLL_CLK     GPIOA,9
#define PLL_DAT     GPIOA,10
#define PLL_LD      GPIOA,7

/* GPS */
#define GPS_TX      GPIOB,6  	//STM32F401 TX1
#define GPS_RX      GPIOB,7  	//STM32F401 RX1

/* OSC */
#define OSC32_IN    GPIOC,14	//STM32F401 OSC32 IN
#define OSC32_OUT   GPIOC,15	//STM32F401 OSC32 OUT

/* USB */
#define USB_DP      GPIOA,12    //STM32F401 USB Data Plus
#define USB_DM      GPIOA,11	//STM32F401 USB Data Minus

/* Serial */
#define SER_RX1     GPIOA,3	//STM32F401 RX2
#define SER_TX1     GPIOA,2	//STM32F401 TX2

/* ISP */
#define SWDIO       GPIOA,13
#define SWCLK       GPIOA,14

#endif

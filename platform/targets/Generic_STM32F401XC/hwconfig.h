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
#define PTT_LED     GPIOB,0
#define SYNC_LED    GPIOB,1
#define ERR_LED     GPIOB,2

/* Display */
#define LCD_RST     GPIOC,13
#define LCD_RS      GPIOC,14
#define LCD_CS      GPIOC,15
#define SPI2_CLK    GPIOB,13
#define SPI2_SDO    GPIOB,14     // UNUSED
#define SPI2_SDI    GPIOB,15

/* Keyboard */
#define KB_ROW0     GPIOB,3
#define KB_ROW1     GPIOB,4
#define KB_ROW2     GPIOB,5
#define KB_ROW3     GPIOB,6
#define KB_ROW4     GPIOB,7
#define KB_COL0     GPIOB,12
#define KB_COL1     GPIOB,10
#define KB_COL2     GPIOB,9
#define KB_COL3     GPIOB,8


#define PTT_SW      GPIOA,11
#define PTT_OUT     GPIOA,10

/* Audio */
#define AUDIO_MIC   GPIOA,2
#define AUDIO_SPK   GPIOA,5
#define BASEBAND_RX GPIOA,1
#define BASEBAND_TX GPIOA,4
#define SPK_MUTE    GPIOA,3
#define MIC_MUTE    GPIOA,0

/* PLL */
#define PLL_CS  GPIOA,6
#define PLL_CLK GPIOA,7
#define PLL_DAT GPIOA,8
#define PLL_LD  GPIOA,9

#endif

/***************************************************************************
 *   Copyright (C) 2021 - 2023 by Federico Amedeo Izzo IU2NUO,             *
 *                                Niccolò Izzo IU2KIN                      *
 *                                Frederik Saraci IU2NRO                   *
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

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <peripherals/gpio.h>
#include <interfaces/display.h>
#include <interfaces/delays.h>
#include <hwconfig.h>
#include <SPI2.h>


/*
 * LCD framebuffer
 * Pixel format is black and white, one bit per pixel.
 */
#define FB_SIZE (((SCREEN_HEIGHT * SCREEN_WIDTH) / 8 ) + 1)
static uint8_t frameBuffer[FB_SIZE];
uint8_t displayChipSelected = false;

void select_display_chip(uint8_t value)
{
    displayChipSelected = value;
    #ifdef LCD_CS
    value ? gpio_clearPin(LCD_CS) : gpio_setPin(LCD_CS);
    #endif
}

/**
 * \internal
 * Send one row of pixels to the display.
 * Pixels in framebuffer are stored "by rows", while display needs data to be
 * sent "by page": this function performs the needed conversion.
 *
 * @param page: page of rows to be be sent.
 */
void display_renderPage(uint8_t page)
{
    for(uint8_t seg = 0; seg < SCREEN_WIDTH; seg++)
    {
        uint8_t out = 0;
        uint8_t tmp;

        for(uint8_t row = 0; row < 8; row++) //step through each of the 8 rows in the page to generate a segment (page-column)
        {
            tmp = frameBuffer[(row + page*8)*SCREEN_WIDTH/8+seg/8];
            out |= ((tmp >> (seg % 8)) & 0x01) << row;
        }
        spi2_sendRecv(out);
    }
}


void display_init()
{
    /* Clear framebuffer, setting all pixels to 0x00 makes the screen white */
    memset(frameBuffer, 0x00, FB_SIZE);

    /*
     * Initialise SPI2 for external flash and LCD
     */
    gpio_setMode(SPI2_CLK, ALTERNATE);
    gpio_setMode(SPI2_SDI, ALTERNATE);
    gpio_setAlternateFunction(SPI2_CLK, 5); /* SPI2 is on AF5 */
    gpio_setAlternateFunction(SPI2_SDI, 5);

    #ifdef SPI2_SDO
    gpio_setMode(SPI2_SDO, ALTERNATE);
    gpio_setAlternateFunction(SPI2_SDO, 5);
    #endif

    spi2_init();

    /*
     * Initialise GPIOs for LCD control
     */
    #ifdef LCD_CS
    gpio_setMode(LCD_CS,  OUTPUT);
    #endif
    gpio_setMode(LCD_RST, OUTPUT);
    gpio_setMode(LCD_RS,  OUTPUT);

    select_display_chip(false);
    gpio_clearPin(LCD_RS);

    gpio_clearPin(LCD_RST); // Reset controller
    delayMs(50);
    gpio_setPin(LCD_RST);
    delayMs(50);

    select_display_chip(true);

    gpio_clearPin(LCD_RS);// RS low -> command mode

    /* Init LCD */
    spi2_sendRecv(0xAE); //Display Off
    spi2_sendRecv(0x20); //Memory Addressing Mode
    spi2_sendRecv(0x10); //Page Addressing Mode
    spi2_sendRecv(0xD3); //Display Offset
    spi2_sendRecv(0x00); //Display Offset
    spi2_sendRecv(0xA1); //Segment re-map
    spi2_sendRecv(0xC8); //COM Output Scan Direction
    spi2_sendRecv(0xDA); //COM Pins Hardware Configuration
    spi2_sendRecv(0x12); //COM Pins Hardware Configuration
    spi2_sendRecv(0x81); //Contrast Control
    spi2_sendRecv(0x7F); //Contrast Control
    spi2_sendRecv(0xA4); //Disable Entire Display On
    spi2_sendRecv(0xA6); //Normal Display
    spi2_sendRecv(0xD5); //Oscillator Frequency
    spi2_sendRecv(0xF0); //Oscillator Frequency
    spi2_sendRecv(0x8D); //Charge Pump Regulator
    spi2_sendRecv(0x14); //Charge Pump Regulator
    spi2_sendRecv(0xAF); //--turn on SSD1306 panel

    select_display_chip(false);
}

void display_terminate()
{
    spi2_terminate();
}

void display_renderPages(uint8_t startPage, uint8_t endPage)
{
    select_display_chip(true);

    for(uint8_t page = startPage; page <= endPage; page++)
    {
        gpio_clearPin(LCD_RS);            /* RS low -> command mode */
        (void) spi2_sendRecv(0xB0 | page); /* Set page (vertical position)         */
        (void) spi2_sendRecv(0x00);       /* Set X position/column/segment         */
        (void) spi2_sendRecv(0x10);       /* to 0 across two commands              */
        gpio_setPin(LCD_RS);              /* RS high -> data mode   */
        display_renderPage(page);
    }

    select_display_chip(false);
}

void display_render()
{
    display_renderPages(0, (SCREEN_HEIGHT / 8) - 1);
}

bool display_renderingInProgress()
{
    return displayChipSelected;
}

void *display_getFrameBuffer()
{
    return (void *)(frameBuffer);
}

void display_setContrast(uint8_t contrast)
{
    select_display_chip(true);

    gpio_clearPin(LCD_RS);             /* RS low -> command mode              */
    (void) spi2_sendRecv(0x81);        /* Set Electronic Volume               */
    (void) spi2_sendRecv(contrast);    /* Controller contrast range is 0 - 63 */

    select_display_chip(false);
}

void display_setBacklightLevel(uint8_t level)
{
    (void) level;
}

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
#include <interfaces/display.h>
#include <interfaces/delays.h>
#include <hwconfig.h>
#include <I2C1.h>


/*
 * LCD framebuffer
 * Pixel format is black and white, one bit per pixel.
 */
#define FB_SIZE (((SCREEN_HEIGHT * SCREEN_WIDTH) / 8 ) + 1)
static uint8_t frameBuffer[FB_SIZE];
bool displayRenderingInProgress;

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
        // this function can be improved by working through the segments in chunks
        // of size matching the array unit (8 bits in this case)
        // we are currently reading only the first bit and discarding the other seven
        // only to re-read the array on the next iteration
        uint8_t out = 0;
        uint8_t tmp;

        for(uint8_t row = 0; row < 8; row++) //step through each of the 8 rows in the page to generate a segment (page-column)
        {
            tmp = frameBuffer[(row + page*8)*SCREEN_WIDTH/8+seg/8];
            out |= ((tmp >> (seg % 8)) & 0x01) << row;
        }
        i2c1_send(0x7B,out,8,true);
    }
}


void display_init()
{
    /* Clear framebuffer, setting all pixels to 0x00 makes the screen white */
    memset(frameBuffer, 0x00, FB_SIZE);

    /* Init LCD */
    i2c1_send(0x7B,0xAE,8,true); //Display Off
    i2c1_send(0x7B,0x20,8,true); //Memory Addressing Mode
    i2c1_send(0x7B,0x10,8,true); //Page Addressing Mode
    i2c1_send(0x7B,0xD3,8,true); //Display Offset
    i2c1_send(0x7B,0x00,8,true); //Display Offset
    i2c1_send(0x7B,0xA1,8,true); //Segment re-map
    i2c1_send(0x7B,0xC8,8,true); //COM Output Scan Direction
    i2c1_send(0x7B,0xDA,8,true); //COM Pins Hardware Configuration
    i2c1_send(0x7B,0x12,8,true); //COM Pins Hardware Configuration
    i2c1_send(0x7B,0x81,8,true); //Contrast Control
    i2c1_send(0x7B,0x7F,8,true); //Contrast Control
    i2c1_send(0x7B,0xA4,8,true); //Disable Entire Display On
    i2c1_send(0x7B,0xA6,8,true); //Normal Display
    i2c1_send(0x7B,0xD5,8,true); //Oscillator Frequency
    i2c1_send(0x7B,0xF0,8,true); //Oscillator Frequency
    i2c1_send(0x7B,0x8D,8,true); //Charge Pump Regulator
    i2c1_send(0x7B,0x14,8,true); //Charge Pump Regulator
    i2c1_send(0x7B,0xAF,8,true); //--turn on SSD1306 panel
}

void display_terminate()
{
    return;
}

void display_renderPages(uint8_t startPage, uint8_t endPage)
{
    displayRenderingInProgress = true;

    for(uint8_t page = startPage; page <= endPage; page++)
    {
        (void) i2c1_send(0x7B,0xB0 | page,8,true); /* Set page (vertical position)         */
        (void) i2c1_send(0x7B,0x00,8,true);       /* Set X position/column/segment         */
        (void) i2c1_send(0x7B,0x10,8,true);       /* to 0 across two commands              */
        display_renderPage(page);
    }
    displayRenderingInProgress = false;
}

void display_render()
{
    display_renderPages(0, (SCREEN_HEIGHT / 8) - 1);
}

bool display_renderingInProgress()
{
    return displayRenderingInProgress;
}

void *display_getFrameBuffer()
{
    return (void *)(frameBuffer);
}

void display_setContrast(uint8_t contrast)
{
    //(void) i2c1_send(0x7B,0x81,8,true);        /* Set Electronic Volume               */
    //(void) i2c1_send(0x7B,contrast,8,true);    /* Controller contrast range is 0 - 63 */
}

void display_setBacklightLevel(uint8_t level)
{
    (void) level;
}

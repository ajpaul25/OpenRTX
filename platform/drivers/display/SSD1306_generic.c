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

/**
 * \internal
 * Send one row of pixels to the display.
 * Pixels in framebuffer are stored "by rows", while display needs data to be
 * sent "by columns": this function performs the needed conversion.
 *
 * @param pixel: pixel row to be be sent.
 */
void display_renderPage(uint8_t page)
{
    for(uint8_t seg = 0; seg < SCREEN_WIDTH; seg++)
    {
        uint8_t out = 0;
        uint8_t tmp;// = frameBuffer[(i * SCREEN_WIDTH/8) + (SCREEN_WIDTH/8 - 1 - page)];

        for(uint8_t row = 0; row < 8; row++) //step through each of the 8 rows in the page to generate a segment (page-column)
        {
            tmp = frameBuffer[(row + page*8)*SCREEN_WIDTH/8+seg/8];
            //out |= ((tmp >> (7-j)) & 0x01) << j;
            out |= ((tmp >> (seg % 8)) & 0x01) << row;
        }
        spi2_sendRecv(out);
    }
}


void display_init()
{
    delayMs(1000);
    gpio_setPin(PTT_OUT);
    delayMs(1000);
    /* Clear framebuffer, setting all pixels to 0x00 makes the screen white */
    memset(frameBuffer, 0x00, FB_SIZE);

    /*
     * Initialise SPI2 for external flash and LCD
     */
    gpio_setMode(SPI2_CLK, ALTERNATE);
    gpio_setMode(SPI2_SDO, ALTERNATE);
    gpio_setMode(SPI2_SDI, ALTERNATE);
    gpio_setAlternateFunction(SPI2_CLK, 5); /* SPI2 is on AF5 */
    gpio_setAlternateFunction(SPI2_SDO, 5);
    gpio_setAlternateFunction(SPI2_SDI, 5);

    spi2_init();

    /*
     * Initialise GPIOs for LCD control
     */
    gpio_setMode(LCD_CS,  OUTPUT);
    gpio_setMode(LCD_RST, OUTPUT);
    gpio_setMode(LCD_RS,  OUTPUT);

    gpio_setPin(LCD_CS);
    gpio_clearPin(LCD_RS);

    gpio_clearPin(LCD_RST); // Reset controller
    delayMs(50);
    gpio_setPin(LCD_RST);
    delayMs(50);

    gpio_clearPin(LCD_CS);

    gpio_clearPin(LCD_RS);// RS low -> command mode

	/* Init LCD */
	spi2_sendRecv(0xAE); //display off
	spi2_sendRecv(0x20); //Set Memory Addressing Mode   
	spi2_sendRecv(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	spi2_sendRecv(0xB0); //Set Page Start Address for Page Addressing Mode,0-7
	spi2_sendRecv(0xC8); //Set COM Output Scan Direction
	spi2_sendRecv(0x00); //---set low column address
	spi2_sendRecv(0x00); //---set high column address
	spi2_sendRecv(0x40); //--set start line address
	spi2_sendRecv(0x81); //--set contrast control register
	spi2_sendRecv(0xFF);
	spi2_sendRecv(0xA1); //--set segment re-map 0 to 127
	spi2_sendRecv(0xA6); //--set normal display
	spi2_sendRecv(0xA8); //--set multiplex ratio(1 to 64)
	spi2_sendRecv(0x3F); //
	spi2_sendRecv(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	spi2_sendRecv(0xD3); //-set display offset
	spi2_sendRecv(0x00); //-not offset
	spi2_sendRecv(0xD5); //--set display clock divide ratio/oscillator frequency
	spi2_sendRecv(0xF0); //--set divide ratio
	spi2_sendRecv(0xD9); //--set pre-charge period
	spi2_sendRecv(0x22); //
	spi2_sendRecv(0xDA); //--set com pins hardware configuration
	spi2_sendRecv(0x12);
	spi2_sendRecv(0xDB); //--set vcomh
	spi2_sendRecv(0x20); //0x20,0.77xVcc
	spi2_sendRecv(0x8D); //--set DC-DC enable
	spi2_sendRecv(0x14); //
	spi2_sendRecv(0xAF); //--turn on SSD1306 panel

/*
    spi2_sendRecv(0xAE);  // SH110X_DISPLAYOFF,
    spi2_sendRecv(0xd5);  // SH110X_SETDISPLAYCLOCKDIV, 0x51,
    spi2_sendRecv(0x51);
    spi2_sendRecv(0x81);  // SH110X_SETCONTRAST, 0x4F,
    spi2_sendRecv(0x4F);
    spi2_sendRecv(0xAD);  // SH110X_DCDC, 0x8A,
    spi2_sendRecv(0x8A);
    spi2_sendRecv(0xA0);  // SH110X_SEGREMAP,
    spi2_sendRecv(0xC0);  // SH110X_COMSCANINC,
    spi2_sendRecv(0xDC);  // SH110X_SETDISPSTARTLINE, 0x0,
    spi2_sendRecv(0x00);
    spi2_sendRecv(0xd3);  // SH110X_SETDISPLAYOFFSET, 0x60,
    spi2_sendRecv(0x60);
    spi2_sendRecv(0xd9);  // SH110X_SETPRECHARGE, 0x22,
    spi2_sendRecv(0x22);
    spi2_sendRecv(0xdb);  // SH110X_SETVCOMDETECT, 0x35,
    spi2_sendRecv(0x35);
    spi2_sendRecv(0xa8);  // SH110X_SETMULTIPLEX, 0x3F,
    spi2_sendRecv(0x3f);
    spi2_sendRecv(0xa4);  // SH110X_DISPLAYALLON_RESUME,
    spi2_sendRecv(0xa6);  // SH110X_NORMALDISPLAY,
    spi2_sendRecv(0xAF);  // SH110x_DISPLAYON*/
    gpio_setPin(LCD_CS);
    display_render();
}

void display_terminate()
{
    spi2_terminate();
}

void display_renderPages(uint8_t startPage, uint8_t endPage)
{
    gpio_clearPin(LCD_CS);

    for(uint8_t page = startPage; page <= endPage; page++)
    {
        gpio_clearPin(LCD_RS);            /* RS low -> command mode */
        (void) spi2_sendRecv(0xB0 | page); /* Set page (vertical position)         */
        (void) spi2_sendRecv(0x00);       /* Set X position/column/segment         */
        (void) spi2_sendRecv(0x10);       /* to 0 across two commands              */
        gpio_setPin(LCD_RS);              /* RS high -> data mode   */
        display_renderPage(page);
    }

    gpio_setPin(LCD_CS);
}

void display_render()
{
    display_renderPages(0, (SCREEN_HEIGHT / 8) - 1);
}

bool display_renderingInProgress()
{
    return (gpio_readPin(LCD_CS) == 0);
}

void *display_getFrameBuffer()
{
    return (void *)(frameBuffer);
}

void display_setContrast(uint8_t contrast)
{
    gpio_clearPin(LCD_CS);

    gpio_clearPin(LCD_RS);             /* RS low -> command mode              */
    (void) spi2_sendRecv(0x81);        /* Set Electronic Volume               */
    (void) spi2_sendRecv(contrast);    /* Controller contrast range is 0 - 63 */

    gpio_setPin(LCD_CS);
}

void display_setBacklightLevel(uint8_t level)
{
    (void) level;
}

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

#ifndef MC145158_H
#define MC145158_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Driver for MC145158 PLL IC.
 *
 */

/**
 * Initialise the PLL.
 */
void MC145158_init();

/**
 * Terminate PLL driver, bringing GPIOs back to reset state.
 */
void MC145158_terminate();

/**
 * Change VCO frequency.
 * @param freq: new VCO frequency, in Hz.
 * @param clkDiv: reference clock division factor.
 */
void MC145158_setFrequency(float freq, uint8_t clkDiv);

/**
 * Check if PLL is locked.
 * @return true if PLL is locked.
 */
bool MC145158_isPllLocked();

#ifdef __cplusplus
}
#endif

#endif /* MC145158_H */

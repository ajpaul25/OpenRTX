/***************************************************************************
 *   Copyright (C) 2022 by Federico Amedeo Izzo IU2NUO,                    *
 *                         Niccolò Izzo IU2KIN                             *
 *                         Frederik Saraci IU2NRO                          *
 *                         Silvano Seva IU2KWO                             *
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

#ifndef RTXLINK_H
#define RTXLINK_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Identifiers for each sub-protocol of rtxlink
 */
enum ProtocolID
{
    RTXLINK_FRAME_STDIO  = 0x00,
    RTXLINK_FRAME_CAT    = 0x01,
    RTXLINK_FRAME_FMP    = 0x02,
    RTXLINK_FRAME_XMODEM = 0x03
};

/**
 * Error codes used in OpenRTX
 */
enum errno
{
    OK       = 0,    // Success
    E2BIG    = 7,    // Argument list too long
    EBADR    = 53,   // Invalid request descriptor
    EBADRQC  = 56,   // Invalid request code
    EGENERIC = 255   // Generic error
};

/**
 * Initialize the rtxlink management module.
 */
void rtxlink_init();

/**
 * Periodic rtxlink update task.
 */
void rtxlink_task();

/**
 * Shut down the rtxlink managment module.
 */
void rtxlink_terminate();

/**
 * Send a block of data over rtxlink.
 *
 * @param proto: protocol ID.
 * @param data: pointer to payload data;
 * @param len: payload length in bytes.
 * @return true on success, false if a transmission is alredy ongoing.
 */
bool rtxlink_send(const enum ProtocolID proto, const void *data, const size_t len);

/**
 * Register a protocol handler callback.
 *
 * @param proto: protocol ID.
 * @param handler: callback function;
 * @return true on success, false if a callback is already registered.
 */
bool rtxlink_setProtocolHandler(const enum ProtocolID proto,
                               void (*handler)(const uint8_t *, size_t));

/**
 * Remove an registered protocol handler callback.
 *
 * @param proto: protocol ID.
 */
void rtxlink_removeProtocolHandler(const enum ProtocolID proto);

#ifdef __cplusplus
}
#endif

#endif

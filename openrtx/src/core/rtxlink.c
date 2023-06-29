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

#include <interfaces/com_port.h>
#include <rtxlink.h>
#include <slip.h>
#include <crc.h>
#include <stdbool.h>
#include <string.h>

typedef void (*protoHandler)(const uint8_t *, size_t);

protoHandler handlers[4] = {NULL};

uint8_t rxDataBuf[144];
uint8_t txDataBuf[260];
size_t  rxDataBufLen = 0;
size_t  txDataBufLen = 0; // Amount of data to be sent out
size_t  txDataBufPos = 0; // Current position of data to be sent out


/**
 * \internal
 * Retrieve and decode a SLIP-encoded data frame from the com port.
 *
 * @return true when a data frame has been completely retrieved.
 */
static bool fetchSlipFrame()
{
    static uint8_t rxBuf[64];
    static ssize_t rxBufLen = 0;
    static ssize_t rxBufPos = 0;

    // Fetch data from com port, exit on error or no data received.
    if(rxBufLen <= 0)
    {
        rxBufPos = 0;
        rxBufLen = com_readBlock(&rxBuf, 64);
        if(rxBufLen <= 0)
            return false;
    }

    // Strip the leading END character(s) before start decoding a new frame
    if(rxDataBufLen == 0)
    {
        while(slip_searchFrameEnd(&rxBuf[rxBufPos], rxBufLen) == 0)
        {
            rxBufPos += 1;
            rxBufLen -= 1;
        }
    }

    // Decode SLIP-encoded data
    ssize_t end      = slip_searchFrameEnd(&rxBuf[rxBufPos], rxBufLen);
    size_t  toDecode = rxBufLen;
    if(end > 0) toDecode = end + 1;

    // Bad, too much bytes
    if((rxDataBufLen + toDecode) > 128)
    {
        rxDataBufLen = 0;
        rxBufLen    -= toDecode;
        return false;
    }

    rxDataBufLen += slip_decodeBlock(&rxBuf[rxBufPos], &rxDataBuf[rxDataBufLen],
                                     toDecode);
    rxBufLen -= toDecode;
    rxBufPos += toDecode;

    // Still some data to receive, wait for next round.
    if(end < 0)
        return false;

    return true;
}


void rtxlink_init()
{

}

void rtxlink_task()
{

    // If we have data to send, finish sending, otherwise receive
    if(txDataBufPos < txDataBufLen)
    {
        size_t toSend = txDataBufLen - txDataBufPos;
        if(toSend > 64) toSend = 64;

        ssize_t sent = com_writeBlock(&txDataBuf[txDataBufPos], toSend);
        if(sent > 0)
            txDataBufPos += sent;
        // If all the data has already been sent
        if(txDataBufPos >= txDataBufLen)
        {
            txDataBufPos = 0;
            txDataBufLen = 0;
        }
    } else {
        if(fetchSlipFrame() == true)
        {
            uint8_t crc = crc_8bit(rxDataBuf, rxDataBufLen - 1);
            if(crc == rxDataBuf[rxDataBufLen - 1])
            {
                uint8_t protocol = rxDataBuf[0];
                uint8_t *data    = &rxDataBuf[1];
                size_t  dataLen  = rxDataBufLen - 2;

                protoHandler handler = handlers[protocol];
                if(handler != NULL)
                    handler(data, dataLen);
            }
        }

        // Flush old data to start fetching a new frame
        rxDataBufLen = 0;
    }

}

void rtxlink_terminate()
{

}

bool rtxlink_send(const enum ProtocolID proto, const void *data, const size_t len)
{
    if(txDataBufLen != 0)
        return false;

    if(len > 128)
        return false;

    uint8_t frame[132];
    frame[0] = proto;
    memcpy(&frame[1], data, len);

    uint8_t crc = crc_8bit(frame, len + 1);
    frame[len + 1] = crc;

    txDataBufLen = slip_encode(frame, txDataBuf, len + 2, true, true);
    txDataBufPos = 0;

    return true;
}

bool rtxlink_setProtocolHandler(const enum ProtocolID proto,
                               void (*handler)(const uint8_t *, size_t))
{
    if(proto > RTXLINK_FRAME_XMODEM)
        return false;

    if(handlers[proto] != NULL)
        return false;

    handlers[proto] = handler;
    return true;
}

void rtxlink_removeProtocolHandler(const enum ProtocolID proto)
{
    if(proto > RTXLINK_FRAME_XMODEM)
        return;

    handlers[proto] = NULL;
}

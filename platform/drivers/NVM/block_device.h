/***************************************************************************
 *   Copyright (C) 2023 by Federico Amedeo Izzo IU2NUO,                    *
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

#ifndef BLOCK_DEVICE_H
#define BLOCK_DEVICE_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum bdType
{
    BD_FLASH,   ///< FLASH block device
    BD_EEPROM   ///< EEPROM block device
};

/**
 * Block device information data.
 * A page is the smallest erasable area on the device.
 */
struct bd_info
{
    const enum bdType type;     ///< Device type
    const size_t page_count;    ///< Number of pages
    const size_t page_size;     ///< Size of a page
    const size_t page_cycles;   ///< Maximum allowed erase cycles of a page
};


/**
 * Driver interface for a generic block device.
 */
struct bd_driver_api
{
    int (*read) (uint32_t offset, void *data, size_t len);
    int (*write)(uint32_t offset, const void *data, size_t len);
    int (*erase)(uint32_t offset, size_t size);
    int (*sync)();
};


/**
 * Block device descriptor.
 */
struct blockDevice
{
    const struct bd_info       *info;  ///< Device info
    const struct bd_driver_api *api;   ///< Device driver API
};


/**
 * Read data from block device.
 *
 * @param dev: block device.
 * @param offset: offset to read, byte aligned.
 * @param data: destination buffer for data read.
 * @param len: number of bytes to read.
 * @return 0 on success, negative errno code on fail.
 */
static inline int blockDevice_read(const struct blockDevice *dev, uint32_t offset,
                                   void *data, size_t len)
{
    return dev->api->read(offset, data, len);
}

/**
 * Write data to block device. On flash memory devices the area has to be
 * erased before the write.
 *
 * @param dev: block device.
 * @param offset: starting offset for the write, byte aligned.
 * @param data: data to write.
 * @param len: number of bytes to write.
 * @return 0 on success, negative errno code on fail.
 */
static inline int blockDevice_write(const struct blockDevice *dev, uint32_t offset,
                                    const void *data, size_t len)
{
    return dev->api->write(offset, data, len);
}

/**
 * Erase part or all of the block device memory.
 * Acceptable values of erase size and offset are subject to hardware-specific
 * multiples of page size and offset.
 *
 * @param dev: block device.
 * @param offset: starting offset for the erase, byte aligned.
 * @param size: size of the area to be erased.
 * @return 0 on success, negative errno code on fail.
 */
static inline int blockDevice_erase(const struct blockDevice *dev, uint32_t offset,
                                    size_t size)
{
    if(dev->api->erase == NULL)
        return 0;

    return dev->api->erase(offset, size);
}

/**
 * Sync block device cache and state to its underlying hardware.
 *
 * @param dev: block device.
 * @return 0 on success, negative errno code on fail.
 */
static inline int blockDevice_sync(const struct blockDevice *dev)
{
    if(dev->api->sync == NULL)
        return 0;

    return dev->api->sync(dev);
}

/**
 * Get the block device information.
 *
 * @param dev: block device.
 * @return a pointer to the block device info descriptor.
 */
static inline const struct bd_info *blockDevice_info(const struct blockDevice *dev)
{
    return dev->info;
}

#ifdef __cplusplus
}
#endif

#endif /* BLOCK_DEVICE_H */

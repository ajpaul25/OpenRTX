/***************************************************************************
 *   Copyright (C) 2023 by Federico Amedeo Izzo IU2NUO,                    *
 *                         Niccolò Izzo IU2KIN                             *
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

#ifndef PMU_H
#define PMU_H

#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/i2c.h>

/*
 * I2C is needed to configure the PMU, check status in devicetree
 */
#if DT_NODE_HAS_STATUS(DT_ALIAS(i2c_0), okay)
#define I2C_DEV_NODE	DT_ALIAS(i2c_0)
#else
#error "Please set the correct I2C device"
#endif

/*
 * PMU is controlled through the XPowersLib external library
 */
#define XPOWERS_CHIP_AXP2101
#include <XPowersLib.h>

/*
 * Initialize PMU to power SA868, LED and GPS
 */
static XPowersPMU PMU;

static const uint32_t i2c_cfg = I2C_SPEED_SET(I2C_SPEED_FAST) | I2C_MODE_CONTROLLER;
static const struct device *const i2c_dev = DEVICE_DT_GET(I2C_DEV_NODE);

int pmu_registerReadByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t len);

int pmu_registerWriteByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t len);

void pmu_init();

void pmu_setBasebandPower(bool value);

void pmu_setGPSPower(bool value);

uint16_t pmu_getVbat();

#ifdef __cplusplus
extern "C" {
#endif

void pmu_handleIRQ();

#ifdef __cplusplus
}
#endif

#endif // PMU_H

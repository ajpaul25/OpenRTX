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

#include <interfaces/platform.h>
#include <interfaces/delays.h>
#include <hwconfig.h>
#include <ui.h>

#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/led_strip.h>

/*
 * I2C is needed to configure the PMU, check status in devicetree
 */
#if DT_NODE_HAS_STATUS(DT_ALIAS(i2c_0), okay)
#define I2C_DEV_NODE	DT_ALIAS(i2c_0)
#else
#error "Please set the correct I2C device"
#endif

static const struct device *const i2c_dev = DEVICE_DT_GET(I2C_DEV_NODE);

/*
 * PMU is controlled through the XPowersLib external library
 */
#define XPOWERS_CHIP_AXP2101
#include <XPowersLib.h>

/*
 * Get buttons devicetree object to access PTT button
 */
#define BUTTON_PTT_NODE	DT_NODELABEL(button_ptt)
static const struct gpio_dt_spec button_ptt = GPIO_DT_SPEC_GET_OR(BUTTON_PTT_NODE, gpios, {0});

/*
 * Initialize PMU to power SA868, LED and GPS
 */
static XPowersPMU PMU;

/*
 * Rotary encoder is read using hardware pulse counter configured as quadrature decoder
 */
static const struct device *const qdec_dev = DEVICE_DT_GET(DT_ALIAS(qdec0));

static const uint32_t i2c_cfg = I2C_SPEED_SET(I2C_SPEED_FAST) | I2C_MODE_CONTROLLER;

/*
 * Initialize WS2812C RGB LED
 */
#define LED_NODE    DT_ALIAS(led0)
#define RGB(_r, _g, _b) { .r = (_r), .g = (_g), .b = (_b) }
struct led_rgb led_color = RGB(0x00, 0x00, 0x00);
static const struct device *const led_dev = DEVICE_DT_GET(LED_NODE);

static hwInfo_t hwInfo =
{
    .name        = "ttwrplus",

    .hw_version  = 0,

    ._unused     = 0,
    .uhf_band    = 0,
    .vhf_band    = 0,

    .uhf_maxFreq = 480,
    .uhf_minFreq = 400,

    .vhf_maxFreq = 174,
    .vhf_minFreq = 134,
};

int pmu_registerReadByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t len)
{
    // Only single-byte reads are supported
    if (len != 1)
        return -1;
    return i2c_reg_read_byte(i2c_dev, devAddr, regAddr, data);
}

int pmu_registerWriteByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t len)
{
    // Only single-byte writes are supported
    if (len != 1)
        return -1;
    return i2c_reg_write_byte(i2c_dev, devAddr, regAddr, *data);
}

void pmu_init()
{
    bool result = PMU.begin(AXP2101_SLAVE_ADDRESS, pmu_registerReadByte, pmu_registerWriteByte);
    if (result == false) {
        while (1) {
            printk("PMU is not online...");
            delayMs(500);
        }
    }

    // Set the minimum common working voltage of the PMU VBUS input,
    // below this value will turn off the PMU
    PMU.setVbusVoltageLimit(XPOWERS_AXP2101_VBUS_VOL_LIM_3V88);

    // Set the maximum current of the PMU VBUS input,
    // higher than this value will turn off the PMU
    PMU.setVbusCurrentLimit(XPOWERS_AXP2101_VBUS_CUR_LIM_2000MA);


    // Get the VSYS shutdown voltage
    uint16_t vol = PMU.getSysPowerDownVoltage();
    printk("->  getSysPowerDownVoltage:%u\n", vol);

    // Set VSY off voltage as 2600mV , Adjustment range 2600mV ~ 3300mV
    PMU.setSysPowerDownVoltage(2600);


    //! DC1 ESP32S3 Core VDD , Don't change
    // PMU.setDC1Voltage(3300);

    //! DC3 Radio & Pixels VDD , Don't change
    PMU.setDC3Voltage(3400);

    //! ALDO2 MICRO TF Card VDD, Don't change
    PMU.setALDO2Voltage(3300);

    //! ALDO4 GNSS VDD, Don't change
    PMU.setALDO4Voltage(3300);

    //! BLDO1 MIC VDD, Don't change
    PMU.setBLDO1Voltage(3300);


    //! The following supply voltages can be controlled by the user
    // DC5 IMAX=2A
    // 1200mV
    // 1400~3700mV,100mV/step,24steps
    PMU.setDC5Voltage(3300);

    //ALDO1 IMAX=300mA
    //500~3500mV, 100mV/step,31steps
    PMU.setALDO1Voltage(3300);

    //ALDO3 IMAX=300mA
    //500~3500mV, 100mV/step,31steps
    PMU.setALDO3Voltage(3300);

    //BLDO2 IMAX=300mA
    //500~3500mV, 100mV/step,31steps
    PMU.setBLDO2Voltage(3300);

    //! END


    // Turn on the power that needs to be used
    //! DC1 ESP32S3 Core VDD , Don't change
    // PMU.enableDC3();

    //! External pin power supply
    PMU.enableDC5();
    PMU.enableALDO1();
    PMU.enableALDO3();
    PMU.enableBLDO2();

    //! ALDO2 MICRO TF Card VDD
    PMU.enableALDO2();

    //! ALDO4 GNSS VDD
    PMU.enableALDO4();

    //! BLDO1 MIC VDD
    PMU.enableBLDO1();

    //! DC3 Radio & Pixels VDD
    PMU.disableDC3();

    // power off when not in use
    PMU.disableDC2();
    PMU.disableDC4();
    PMU.disableCPUSLDO();
    PMU.disableDLDO1();
    PMU.disableDLDO2();



    printk("DCDC=======================================================================\n");
    printk("DC1  : %s   Voltage:%u mV \n",  PMU.isEnableDC1()  ? "+" : "-", PMU.getDC1Voltage());
    printk("DC2  : %s   Voltage:%u mV \n",  PMU.isEnableDC2()  ? "+" : "-", PMU.getDC2Voltage());
    printk("DC3  : %s   Voltage:%u mV \n",  PMU.isEnableDC3()  ? "+" : "-", PMU.getDC3Voltage());
    printk("DC4  : %s   Voltage:%u mV \n",  PMU.isEnableDC4()  ? "+" : "-", PMU.getDC4Voltage());
    printk("DC5  : %s   Voltage:%u mV \n",  PMU.isEnableDC5()  ? "+" : "-", PMU.getDC5Voltage());
    printk("ALDO=======================================================================\n");
    printk("ALDO1: %s   Voltage:%u mV\n",  PMU.isEnableALDO1()  ? "+" : "-", PMU.getALDO1Voltage());
    printk("ALDO2: %s   Voltage:%u mV\n",  PMU.isEnableALDO2()  ? "+" : "-", PMU.getALDO2Voltage());
    printk("ALDO3: %s   Voltage:%u mV\n",  PMU.isEnableALDO3()  ? "+" : "-", PMU.getALDO3Voltage());
    printk("ALDO4: %s   Voltage:%u mV\n",  PMU.isEnableALDO4()  ? "+" : "-", PMU.getALDO4Voltage());
    printk("BLDO=======================================================================\n");
    printk("BLDO1: %s   Voltage:%u mV\n",  PMU.isEnableBLDO1()  ? "+" : "-", PMU.getBLDO1Voltage());
    printk("BLDO2: %s   Voltage:%u mV\n",  PMU.isEnableBLDO2()  ? "+" : "-", PMU.getBLDO2Voltage());
    printk("===========================================================================\n");

    // Set the time of pressing the button to turn off
    PMU.setPowerKeyPressOffTime(XPOWERS_POWEROFF_4S);
    uint8_t opt = PMU.getPowerKeyPressOffTime();
    printk("PowerKeyPressOffTime:");
    switch (opt) {
    case XPOWERS_POWEROFF_4S: printk("4 Second");
        break;
    case XPOWERS_POWEROFF_6S: printk("6 Second");
        break;
    case XPOWERS_POWEROFF_8S: printk("8 Second");
        break;
    case XPOWERS_POWEROFF_10S: printk("10 Second");
        break;
    default:
        break;
    }
    printk("\n");
    // Set the button power-on press time
    PMU.setPowerKeyPressOnTime(XPOWERS_POWERON_128MS);
    opt = PMU.getPowerKeyPressOnTime();
    printk("PowerKeyPressOnTime:");
    switch (opt) {
    case XPOWERS_POWERON_128MS: printk("128 Ms");
        break;
    case XPOWERS_POWERON_512MS: printk("512 Ms");
        break;
    case XPOWERS_POWERON_1S: printk("1 Second");
        break;
    case XPOWERS_POWERON_2S: printk("2 Second");
        break;
    default:
        break;
    }
    printk("\n");

    printk("===========================================================================\n");
    // It is necessary to disable the detection function of the TS pin on the board
    // without the battery temperature detection function, otherwise it will cause abnormal charging
    PMU.disableTSPinMeasure();


    // Enable internal ADC detection
    PMU.enableBattDetection();
    PMU.enableVbusVoltageMeasure();
    PMU.enableBattVoltageMeasure();
    PMU.enableSystemVoltageMeasure();


    /*
      The default setting is CHGLED is automatically controlled by the PMU.
    - XPOWERS_CHG_LED_OFF,
    - XPOWERS_CHG_LED_BLINK_1HZ,
    - XPOWERS_CHG_LED_BLINK_4HZ,
    - XPOWERS_CHG_LED_ON,
    - XPOWERS_CHG_LED_CTRL_CHG,
    * */
    PMU.setChargingLedMode(XPOWERS_CHG_LED_CTRL_CHG);

    // TODO: Implement IRQ 
    // pinMode(PMU_IRQ, INPUT_PULLUP);
    // attachInterrupt(PMU_IRQ, setFlag, FALLING);

    // Disable all interrupts
    PMU.disableIRQ(XPOWERS_AXP2101_ALL_IRQ);
    // Clear all interrupt flags
    PMU.clearIrqStatus();
    // Enable the required interrupt function
    PMU.enableIRQ(
        XPOWERS_AXP2101_BAT_INSERT_IRQ    | XPOWERS_AXP2101_BAT_REMOVE_IRQ      |   //BATTERY
        XPOWERS_AXP2101_VBUS_INSERT_IRQ   | XPOWERS_AXP2101_VBUS_REMOVE_IRQ     |   //VBUS
        XPOWERS_AXP2101_PKEY_SHORT_IRQ    | XPOWERS_AXP2101_PKEY_LONG_IRQ       |   //POWER KEY
        XPOWERS_AXP2101_BAT_CHG_DONE_IRQ  | XPOWERS_AXP2101_BAT_CHG_START_IRQ       //CHARGE
    );

    // Set the precharge charging current
    PMU.setPrechargeCurr(XPOWERS_AXP2101_PRECHARGE_150MA);

    // Set constant current charge current limit
    //! Using inferior USB cables and adapters will not reach the maximum charging current.
    //! Please pay attention to add a suitable heat sink above the PMU when setting the charging current to 1A
    // NOTE: Charging current set to 500mAh to remove the need for a heat sink
    PMU.setChargerConstantCurr(XPOWERS_AXP2101_CHG_CUR_500MA);

    // Set stop charging termination current
    PMU.setChargerTerminationCurr(XPOWERS_AXP2101_CHG_ITERM_150MA);

    // Set charge cut-off voltage
    // NOTE: Target voltage set to 4.00V (80% charge) to extend battery lifespan of 2.5x-3x
    PMU.setChargeTargetVoltage(XPOWERS_AXP2101_CHG_VOL_4V);

    // Disable the PMU long press shutdown function
    //PMU.disableLongPressShutdown();

    // Get charging target current
    const uint16_t currTable[] = {
        0, 0, 0, 0, 100, 125, 150, 175, 200, 300, 400, 500, 600, 700, 800, 900, 1000
    };
    uint8_t val = PMU.getChargerConstantCurr();
    printk("Val = %d\n", val);
    printk("Setting Charge Target Current : %d\n", currTable[val]);

    // Get charging target voltage
    const uint16_t tableVoltage[] = {
        0, 4000, 4100, 4200, 4350, 4400, 255
    };
    val = PMU.getChargeTargetVoltage();
    printk("Setting Charge Target Voltage : %d\n", tableVoltage[val]);

}

void pmu_setBasebandPower(bool value)
{
    if (value)
        PMU.enableDC3();
    else
        PMU.disableDC3();
}

void platform_init()
{
    int ret = 0;
    // Initialize GPIO for PTT and rotary encoder
    if (!gpio_is_ready_dt(&button_ptt)) {
        printk("Error: button device %s is not ready\n",
               button_ptt.port->name);
    }
    ret = gpio_pin_configure_dt(&button_ptt, GPIO_INPUT);
    if (ret != 0) {
        printk("Error %d: failed to configure %s pin %d\n",
               ret, button_ptt.port->name, button_ptt.pin);
    }
    // Rotary encoder is read using hardware pulse counter
    if (!device_is_ready(qdec_dev)) {
        printk("Qdec device is not ready\n");
    }
    // Configure I2C
    if (!device_is_ready(i2c_dev)) {
        printk("I2C device is not ready\n");
    }
    if (i2c_configure(i2c_dev, i2c_cfg)) {
        printk("I2C config failed\n");
    }
    // Initialize PMU
    pmu_init();
    // Initialize LED to off state
    if (!device_is_ready(led_dev)) {
        printk("LED device %s is not ready", led_dev->name);
    }
    ret = led_strip_update_rgb(led_dev, &led_color, 1);
    if (ret) {
        printk("couldn't update strip: %d", ret);
    }
}

void platform_terminate()
{
}

uint16_t platform_getVbat()
{
    return PMU.isBatteryConnect() ? PMU.getBattVoltage() : 0;
}

uint8_t platform_getMicLevel()
{
    return 0;
}

uint8_t platform_getVolumeLevel()
{
    return 0;
}

uint8_t platform_getChSelector()
{
    struct sensor_value val;
    int rc = sensor_sample_fetch(qdec_dev);
    if (rc != 0) {
        printk("Failed to fetch sample (%d)\n", rc);
        return 0;
    }
    rc = sensor_channel_get(qdec_dev, SENSOR_CHAN_ROTATION, &val);
    if (rc != 0) {
        printk("Failed to get data (%d)\n", rc);
        return 0;
    }
    return (val.val1 * (int32_t) 255) / 360 % 255;
}

bool platform_getPttStatus()
{
    return gpio_pin_get_dt(&button_ptt);
}

bool platform_pwrButtonStatus()
{
    return true;
}

void platform_ledOn(led_t led)
{
    int ret = 0;
    switch(led)
    {
        case GREEN:
            led_color.g = 0xff;
            break;
        case RED:
            led_color.r = 0xff;
            break;
        default:
            break;
    }
    ret = led_strip_update_rgb(led_dev, &led_color, 1);
    if (ret) {
        printk("couldn't update strip: %d", ret);
    }
}

void platform_ledOff(led_t led)
{
    int ret = 0;
    switch(led)
    {
        case GREEN:
            led_color.g = 0x00;
            break;
        case RED:
            led_color.r = 0x00;
            break;
        default:
            break;
    }
    ret = led_strip_update_rgb(led_dev, &led_color, 1);
    if (ret) {
        printk("couldn't update strip: %d", ret);
    }
}

void platform_beepStart(uint16_t freq)
{
    (void) freq;
}

void platform_beepStop()
{
    ;
}

hwInfo_t *platform_getHwInfo()
{
    return &hwInfo;
}


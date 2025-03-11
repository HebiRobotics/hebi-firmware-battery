#include "BQ34Z100_I2C.h"


namespace hebi::firmware::hardware {

static THD_WORKING_AREA(batteryThreadWorkingArea, 256);
static THD_FUNCTION(batteryUpdateThread, arg);

static THD_FUNCTION(batteryUpdateThread, arg) {

    BQ34Z100_I2C* ref = (BQ34Z100_I2C*) arg;
    while (true) {
        systime_t time = chVTGetSystemTimeX(); // T0

        ref->update();

        chThdSleepUntilWindowed(time, chTimeAddX(time, TIME_MS2I(500)));
    }

    return;
}

BQ34Z100_I2C::BQ34Z100_I2C(I2CDriver * const driver, const I2CConfig &config) :
driver_(driver), config_(config) {

    // Start thread that continuously polls batteries
    chThdCreateStatic(
        batteryThreadWorkingArea,
        sizeof (batteryThreadWorkingArea),
        NORMALPRIO - 1, /* Initial priority.    */
        batteryUpdateThread, /* Thread function.     */
        this); /* Thread parameter.    */
}

bool BQ34Z100_I2C::readRegister(uint8_t addr, uint16_t& data){
    uint8_t tx_data[1] = {addr};
    uint8_t rx_data[1] = {0};
    msg_t msg = MSG_OK;

    i2cAcquireBus(driver_);
    i2cStart(driver_, &config_);
    
    msg = i2cMasterTransmitTimeout(driver_, BAT_ADDR, tx_data, 1, rx_data, 1, TIME_MS2I(1));

    i2cStop(driver_);
    i2cReleaseBus(driver_);

    data = (uint16_t) rx_data[0];

    return msg == MSG_OK;
}

bool BQ34Z100_I2C::readRegister2(uint8_t addr, uint16_t& data){
    uint8_t tx_data[2] = {addr, addr+1};
    uint8_t rx_data[2] = {0, 0};
    msg_t msg = MSG_OK;

    i2cAcquireBus(driver_);
    i2cStart(driver_, &config_);
    
    msg = i2cMasterTransmitTimeout(driver_, BAT_ADDR, tx_data, 1, rx_data, 1, TIME_MS2I(1));

    if(msg == MSG_OK){
        msg = i2cMasterTransmitTimeout(driver_, BAT_ADDR, tx_data + 1, 1, rx_data + 1, 1, TIME_MS2I(1));
        data = (uint16_t) rx_data[0] | ((uint16_t) rx_data[1] << 8);
    }

    i2cStop(driver_);
    i2cReleaseBus(driver_);

    return msg == MSG_OK;
}

void BQ34Z100_I2C::update(){
    battery_data tmp = {};
    bool success = true;

    success &= readRegister2(VOLTAGE_REG, tmp.voltage);
    success &= readRegister(SOC_REG, tmp.soc);
    success &= readRegister2(CURRENT_REG, (uint16_t &)tmp.current);
    success &= readRegister2(CAP_REM_REG, tmp.capacity_remaining);
    success &= readRegister2(CAP_FUL_REG, tmp.capacity_full);

    if(success){
        chSysLock();
        has_data_ = true;
        data_ = tmp;
        chSysUnlock();
    }
}

    
}
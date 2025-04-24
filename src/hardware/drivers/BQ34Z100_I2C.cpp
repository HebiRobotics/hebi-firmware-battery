#include "BQ34Z100_I2C.h"

/*  App Note: https://www.ti.com/lit/an/slua790/slua790.pdf
    Datasheet: https://www.ti.com/lit/ds/symlink/bq34z100.pdf
*/

namespace hebi::firmware::hardware {

static THD_WORKING_AREA(batteryThreadWorkingArea, 512);
static THD_FUNCTION(batteryUpdateThread, arg);

static THD_FUNCTION(batteryUpdateThread, arg) {

    BQ34Z100_I2C* ref = (BQ34Z100_I2C*) arg;
    while (true) {
        systime_t time = chVTGetSystemTimeX(); // T0

        ref->update();

        chThdSleepUntilWindowed(time, chTimeAddX(time, TIME_MS2I(100)));
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

void BQ34Z100_I2C::startDriver(){
    //Do Nothing?
}

void BQ34Z100_I2C::stopDriver(){
    //Do Nothing?
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

bool BQ34Z100_I2C::readControlRegister(uint16_t addr, uint16_t& data){
    msg_t msg = MSG_OK;
    //Little endian... low byte to CONTROL_REG and high byte to CONTROL_REG + 1
    uint8_t subcommand_write[3] = {CONTROL_REG, addr & 0xFF, (addr >> 8) & 0xFF};
    uint8_t subcommand_read[1] = {CONTROL_REG};

    i2cAcquireBus(driver_);
    i2cStart(driver_, &config_);
    
    msg = i2cMasterTransmitTimeout(driver_, BAT_ADDR, subcommand_write, 3, 0, 0, TIME_MS2I(1));
    
    if(msg == MSG_OK){
        msg = i2cMasterTransmitTimeout(driver_, BAT_ADDR, subcommand_read, 1, 0, 0, TIME_MS2I(1));
    }

    i2cStop(driver_);
    i2cReleaseBus(driver_);

    if(msg != MSG_OK) return false;

    return readRegister2(CONTROL_REG, data);
}

void BQ34Z100_I2C::update(){
    read_count_ ++;
    read_count_ %= 10;
    
    /*  Read at 100ms intervals, 1s period for full data.
        We want to read at least once every 100ms for presence detection updates
        Battery data doesn't update any faster than once per second
    */
    switch (read_count_) { 
    case 0:
        data_tmp_ = {};
        battery_present_ = readRegister2(VOLTAGE_REG, data_tmp_.voltage);
        battery_present_ &= readRegister(SOC_REG, data_tmp_.soc);
        break;
    case 1:
        battery_present_ = readRegister2(CURRENT_REG, (uint16_t &)data_tmp_.current);
        battery_present_ &= readRegister2(TEMP_REG, data_tmp_.temperature);
        break;
    case 2:
        //TODO: This reg fails to ACK?
        //battery_present_ = readRegister2(MAX_LOAD_CURRENT_REG, (uint16_t &)data_tmp_.max_load_current);
        battery_present_ = readRegister2(AVG_CURRENT_REG, (uint16_t &)data_tmp_.avg_current);
        break;
    case 3:
        battery_present_ = readRegister2(STANDBY_CURRENT_REG, (uint16_t &)data_tmp_.standby_current);
        break;
    case 4:
        //TODO: This reg fails to ACK?
        // battery_present_ = readRegister2(SELF_DISCHARGE_CURRENT_REG, (uint16_t &)data_tmp_.self_dsg_current);
        battery_present_ = readRegister2(FLAGS_REG, (uint16_t &)data_tmp_.status_flags);
        break;
    case 5:
        battery_present_ = readRegister2(AVG_POWER_REG, data_tmp_.avg_power);
        break;
    case 6:
        battery_present_ = readRegister2(TIME_TO_EMPTY_REG, data_tmp_.time_to_empty);
        break;
    case 7:
        battery_present_ = readRegister2(TIME_TO_FULL_REG, data_tmp_.time_to_full);
        break;
    case 8:
        battery_present_ = readRegister2(CAPACITY_REMAINING_REG, data_tmp_.capacity_remaining);
        break;
    case 9:
        battery_present_ = readRegister2(CAPACITY_FULL_REG, data_tmp_.capacity_full);
        if(battery_present_){
            chSysLock();
            has_data_ = true;
            data_ = data_tmp_;
            chSysUnlock();
        }
        break;
    }

    // data_tmp_ = {};
    // battery_present_ = readRegister2(VOLTAGE_REG, data_tmp_.voltage);
    // battery_present_ &= readRegister(SOC_REG, data_tmp_.soc);
    // battery_present_ &= readRegister2(CURRENT_REG, (uint16_t &)data_tmp_.current);
    // battery_present_ &= readRegister2(TEMP_REG, data_tmp_.temperature);
    // // battery_present_ &= readRegister2(MAX_LOAD_CURRENT_REG, (uint16_t &)data_tmp_.max_load_current);
    // battery_present_ &= readRegister2(STANDBY_CURRENT_REG, (uint16_t &)data_tmp_.standby_current);
    // // battery_present_ &= readRegister2(SELF_DISCHARGE_CURRENT_REG, (uint16_t &)data_tmp_.self_dsg_current);
    // battery_present_ &= readRegister2(AVG_POWER_REG, data_tmp_.avg_power);
    // battery_present_ &= readRegister2(TIME_TO_EMPTY_REG, data_tmp_.time_to_empty);
    // battery_present_ &= readRegister2(TIME_TO_FULL_REG, data_tmp_.time_to_full);
    // battery_present_ &= readRegister2(CAPACITY_REMAINING_REG, data_tmp_.capacity_remaining);
    // battery_present_ &= readRegister2(CAPACITY_FULL_REG, data_tmp_.capacity_full);

    // if(battery_present_){
    //     chSysLock();
    //     has_data_ = true;
    //     data_ = data_tmp_;
    //     chSysUnlock();
    // }

    // battery_data tmp = {};
    // bool success = true;

    // success &= readRegister2(VOLTAGE_REG, tmp.voltage);
    // success &= readRegister(SOC_REG, tmp.soc);
    // success &= readRegister2(CURRENT_REG, (uint16_t &)tmp.current);
    // success &= readRegister2(CAP_REM_REG, tmp.capacity_remaining);
    // success &= readRegister2(CAP_FUL_REG, tmp.capacity_full);
    // success &= readRegister2(TEMP_REG, tmp.temperature);

    // if(success){
    //     chSysLock();
    //     has_data_ = true;
    //     data_ = tmp;
    //     chSysUnlock();
    // }
}

    
}
#include "battery_comms.h"
#include "pin_definitions.h"


namespace biorobotics {
namespace firmware {
namespace hardware {

static THD_WORKING_AREA(batteryThreadWorkingArea, 256);
static THD_FUNCTION(batteryUpdateThread, arg);

static THD_FUNCTION(batteryUpdateThread, arg) {

    BatteryComms* ref = (BatteryComms*) arg;
    while (true) {
        systime_t time = chVTGetSystemTimeX(); // T0

        ref->Update();

        chThdSleepUntilWindowed(time, chTimeAddX(time, TIME_MS2I(10)));
    }

    return;
}

void BatteryComms::resetI2C(){
    palSetPadMode(BAT_I2C_RESET.port, BAT_I2C_RESET.pin, PAL_MODE_OUTPUT_PUSHPULL);

    palSetPadMode(BAT_SDA.port, BAT_SDA.pin, PAL_MODE_INPUT); //I2C3 SDA
    palSetPadMode(BAT_SCL.port, BAT_SCL.pin, PAL_MODE_INPUT); //I2C3 SCL
    
    palWritePad(BAT_I2C_RESET.port, BAT_I2C_RESET.pin, PAL_LOW);

    chThdSleepMilliseconds(1);
    
    palWritePad(BAT_I2C_RESET.port, BAT_I2C_RESET.pin, PAL_HIGH);
    
    palSetPadMode(BAT_SDA.port, BAT_SDA.pin, PAL_STM32_MODE_ALTERNATE|PAL_STM32_ALTERNATE(4)| PAL_STM32_OTYPE_OPENDRAIN); //I2C3 SDA
    palSetPadMode(BAT_SCL.port, BAT_SCL.pin, PAL_STM32_MODE_ALTERNATE|PAL_STM32_ALTERNATE(4)| PAL_STM32_OTYPE_OPENDRAIN); //I2C3 SCL
}

BatteryComms::BatteryComms(I2CDriver * const driver, const I2CConfig &config) :
driver_(driver), config_(config) {

    //Setup I2C
    resetI2C();

    // Start thread that continuously polls batteries
    chThdCreateStatic(
        batteryThreadWorkingArea,
        sizeof (batteryThreadWorkingArea),
        NORMALPRIO - 1, /* Initial priority.    */
        batteryUpdateThread, /* Thread function.     */
        this); /* Thread parameter.    */
}


bool BatteryComms::setMux(uint8_t ind){
    uint8_t tx_data[1] = { ind };

    i2cAcquireBus(driver_);
    i2cStart(driver_, &config_);

    msg_t msg = i2cMasterTransmitTimeout(driver_, MUX_ADDR, tx_data, 1, NULL, 0, TIME_MS2I(10));

    i2cStop(driver_);
    i2cReleaseBus(driver_);

    return msg == MSG_OK;
}

bool BatteryComms::readRegister(uint8_t addr, uint16_t& data){
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

bool BatteryComms::readRegister2(uint8_t addr, uint16_t& data){
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

void BatteryComms::Update(){
    static uint8_t which_register = 0;
    static uint8_t which_battery = 0;

    which_battery++;
    which_battery %= 4;

    if(which_battery == 0){
        which_register++;
        which_register %= 3;
    }

    chSysLock();
    modules::Input_data tmp = input_data_;
    chSysUnlock();

    if(tmp.v_bat[which_battery] > 20.){
        if(presence_[which_battery] < PRESENCE_COUNT)
            presence_[which_battery]++;
    } else {
        presence_[which_battery] = 0;
    }

    bool success = true;
    if(presence_[which_battery] == PRESENCE_COUNT){
        success = setMux(0x01 << which_battery);
        if(success){
            switch(which_register){
                case 0:
                    success = readRegister2(VOLTAGE_REG, battery_data_[which_battery].voltage);
                    break;
                case 1:
                    success = readRegister(SOC_REG, battery_data_[which_battery].soc);
                    break;
                case 2:
                    success = readRegister2(CURRENT_REG, (uint16_t &)battery_data_[which_battery].current);
                    break;
                default:
                    break;
            }
        }

        if(!success){
            battery_data_[which_battery] = {};                
        }
    } else {
        success = setMux(0);
        battery_data_[which_battery] = {};    
    }

    //If we fail a read, the interface may be locked up. Reset it.
    if(!success){ 
        resetI2C();
    }
}
    
} /* namespace driver */
} /* namespace firmware */
} /* namespace biorobotics */
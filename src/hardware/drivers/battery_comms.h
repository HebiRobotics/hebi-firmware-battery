#pragma once

#include "hardware/ChibiOS.h"
#include "hal.h"
#include "sensor_data.h"

namespace biorobotics {
namespace firmware {
namespace hardware {

struct battery_data {
    int16_t current {0};
    uint16_t soc {0};
    uint16_t voltage {0};
};

class BatteryComms {
public:
    BatteryComms(I2CDriver * const driver, const I2CConfig &config);

    void Update();

    bool batteryPresent(uint8_t ind){
        if(ind < N_BATTERIES)
            return presence_[ind] == PRESENCE_COUNT && battery_data_[ind].voltage != 0;
        return false;
    }

    void updateInputData(modules::Input_data data){
        chSysLock();
        input_data_ = data;
        chSysUnlock();
    }

    uint16_t stateOfCharge(uint8_t ind){
        if(ind < N_BATTERIES)
            return battery_data_[ind].soc;
        return 0;
    }

    int16_t avgCurrent(uint8_t ind){
        if(ind < N_BATTERIES)
            return battery_data_[ind].current;
        return 0;
    }


private:
    bool setMux(uint8_t ind);
    bool readRegister(uint8_t addr, uint16_t& data);
    bool readRegister2(uint8_t addr, uint16_t& data);
    void resetI2C();

    static const uint8_t MUX_ADDR = 0x70;
    static const uint8_t BAT_ADDR = 0x55;

    static const uint8_t VOLTAGE_REG = 0x08; //2 word wide
    static const uint8_t SOC_REG = 0x02; //1 word wide
    static const uint8_t CURRENT_REG = 0x10; //2 word wide

    static const uint8_t N_BATTERIES = 4;
    static const uint16_t PRESENCE_COUNT = 5;

    battery_data battery_data_[N_BATTERIES] {};
    uint16_t presence_[N_BATTERIES] {};

    modules::Input_data input_data_;

    I2CDriver * const driver_;
    const I2CConfig &config_;
};

} /* namespace driver */
} /* namespace firmware */
} /* namespace biorobotics */
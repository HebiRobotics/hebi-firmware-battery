#pragma once

#include "hal.h"
#include "Driver.h"

namespace hebi::firmware::hardware {

struct battery_data {
    uint16_t voltage {0};
    int16_t current {0};
    uint16_t soc {0};
    uint16_t capacity_remaining {0};
    uint16_t capacity_full {0};
    uint16_t temperature {0};
};

class BQ34Z100_I2C : public Driver {
public:
    BQ34Z100_I2C(I2CDriver * const driver, const I2CConfig &config);

    void update();

    bool hasData() { return has_data_; }
    
    battery_data getData(){
        battery_data tmp {};

        chSysLock();
        has_data_ = false;
        tmp = data_;
        chSysUnlock();

        return tmp;
    }

    void startDriver();
    void stopDriver();

private:
    bool readRegister(uint8_t addr, uint16_t& data);
    bool readRegister2(uint8_t addr, uint16_t& data);

    static const uint8_t BAT_ADDR = 0x55;

    static const uint8_t VOLTAGE_REG = 0x08; //2 word wide
    static const uint8_t CURRENT_REG = 0x10; //2 word wide
    static const uint8_t SOC_REG = 0x02; //1 word wide
    static const uint8_t CAP_REM_REG = 0x04; //2 word wide
    static const uint8_t CAP_FUL_REG = 0x06; //2 word wide
    static const uint8_t TEMP_REG = 0x0C; //2 word wide

    bool has_data_ {false};
    battery_data data_{};

    I2CDriver * const driver_;
    const I2CConfig &config_;
};

}
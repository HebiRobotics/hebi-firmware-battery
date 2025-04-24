#pragma once

#include "hal.h"
#include "Driver.h"

namespace hebi::firmware::hardware {

struct battery_data {
    //battery_state_msg
    uint16_t voltage {0};
    int16_t current {0};
    uint16_t soc {0};
    uint16_t temperature {0};

    //battery_state_msg_ext_1
    uint16_t avg_current {0};
    uint16_t standby_current {0};
    uint16_t status_flags {0};
    uint16_t avg_power {0};

    //battery_state_msg_ext_2
    uint16_t time_to_empty {0};
    uint16_t time_to_full {0};
    uint16_t capacity_remaining {0};
    uint16_t capacity_full {0};

    float voltageFloat() const {
        return (float) voltage / 1000.;
    }
    
    float currentFloat() const {
        return (float) current / 1000.;
    }

    bool fullyCharged() const {
        return soc == 100;
    }
};

class BQ34Z100_I2C : public Driver {
public:
    BQ34Z100_I2C(I2CDriver * const driver, const I2CConfig &config);

    void update();

    bool hasData() { return has_data_; }
    bool batteryPresent() { return battery_present_; }
    
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
    bool readControlRegister(uint16_t addr, uint16_t& data);

    static const uint8_t BAT_ADDR = 0x55;

    static const uint8_t CONTROL_REG = 0x00; //2 word wide
    static const uint16_t CONTROL_CURRENT_REG = 0x0018; //2 word wide
    
    //Standard
    static const uint8_t SOC_REG = 0x02; //1 word wide
    static const uint8_t CAPACITY_REMAINING_REG = 0x04; //2 word wide
    static const uint8_t CAPACITY_FULL_REG = 0x06; //2 word wide
    static const uint8_t TEMP_REG = 0x0C; //2 word wide
    static const uint8_t VOLTAGE_REG = 0x08; //2 word wide
    static const uint8_t AVG_CURRENT_REG = 0x0A; //2 word wide
    static const uint8_t FLAGS_REG = 0x0A; //2 word wide

    //Extended
    static const uint8_t CURRENT_REG = 0x10; //2 word wide
    static const uint8_t TIME_TO_EMPTY_REG = 0x18; //2 word wide
    static const uint8_t TIME_TO_FULL_REG = 0x1A; //2 word wide
    static const uint8_t STANDBY_CURRENT_REG = 0x1C; //2 word wide
    static const uint8_t MAX_LOAD_CURRENT_REG = 0x20; //2 word wide - NACK on read?
    static const uint8_t AVG_POWER_REG = 0x26; //2 word wide
    static const uint8_t SELF_DISCHARGE_CURRENT_REG = 0x38; //2 word wide - NACK on read?


    bool battery_present_ {false};
    bool has_data_ {false};
    battery_data data_ {};
    battery_data data_tmp_ {};
    uint8_t read_count_ {};

    I2CDriver * const driver_;
    const I2CConfig &config_;
};

}
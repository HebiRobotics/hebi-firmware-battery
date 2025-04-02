/**
 * LED_RGB.h
 * 
 * Simple interface class for a single-frequency speaker
*/

#pragma once

#include <stdint.h>
#include "hardware/Beeper.h"
#include <limits>

namespace hebi::firmware::modules {

class Beep_Controller {
public:
    Beep_Controller(hardware::Beeper& beeper_driver);

    void update();
    void setCode(  
        uint16_t on_1 = 0, uint16_t off_1 = 0, 
        uint16_t on_2 = 0, uint16_t off_2 = 0, 
        uint16_t on_3 = 0, uint16_t off_3 = 0, 
        uint16_t on_4 = 0, uint16_t off_4 = 0, 
        uint16_t on_5 = 0, uint16_t off_5 = 0 
    );

    static const uint16_t QUICK_BEEP_MS = 100;

    bool active() { return code_index_ < code_length_; }
    void beepOnce(uint16_t time_ms = QUICK_BEEP_MS) { setCode(time_ms, 1); }
    void beepTwice(uint16_t time_ms = QUICK_BEEP_MS) { setCode(time_ms, time_ms, time_ms, 1); }
    void beepThrice(uint16_t time_ms = QUICK_BEEP_MS) { setCode(time_ms, time_ms, time_ms, time_ms, time_ms, 1); }
    void beepFault(uint16_t time_ms = QUICK_BEEP_MS) { setCode(time_ms, time_ms, time_ms, time_ms, time_ms, 3*time_ms); }
protected:
    uint16_t trim(uint16_t time);

    static const uint16_t MAX_CODE_LEN = 10;
    static const uint16_t T_MIN = 0;
    static const uint16_t T_MAX = std::numeric_limits<uint16_t>::max();

    uint16_t counter_ {0};
    uint16_t code_index_ {MAX_CODE_LEN};
    uint16_t code_length_ {MAX_CODE_LEN};
    uint16_t beep_code_[MAX_CODE_LEN] {};

    hardware::Beeper& beeper_driver_;
};

} //namespace hebi::firmware::hardware
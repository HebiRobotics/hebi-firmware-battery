/**
 * ADC_control.h
 * 
 * Driver for ADC with low power shutoff and internal OP-AMP
*/

#pragma once

#include "Driver.h"
#include <cstdint>

namespace hebi::firmware::hardware {

class ADC_control : public Driver {
public:
    ADC_control();

    static void dataUpdated(bool which);
    float v_bat();
    float v_ext();
    
    void startDriver();
    void stopDriver();
private:
    static uint8_t v_bat_which_index_;
    static uint8_t v_ext_which_index_;
};

} //namespace hebi::firmware::hardware
 
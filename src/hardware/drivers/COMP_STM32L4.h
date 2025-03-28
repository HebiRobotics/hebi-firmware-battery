/**
 * COMP_STM32L4.h
 * 
 * Driver for built STM32L432xx built in comparators
*/

#pragma once

#include "Driver.h"

namespace hebi::firmware::hardware {

class COMP_STM32L4 : public Driver {
public:
    COMP_STM32L4();

    bool output_comp1();
    bool output_comp2();
    
    void startDriver();
    void stopDriver();
};

} //namespace hebi::firmware::hardware
 
/**
 * Power_Control.h
 * 
 * Low power control driver
*/

#pragma once

#include "Driver.h"
#include <vector>

extern "C" {
    #include <ch.h>
    #include <hal.h>
}

namespace hebi::firmware::hardware {
 
class Power_Control {
public:
    Power_Control(std::vector<Driver *>& drivers);

    void enterStandby();
    void enterStop2();
    void clearStandby();
    bool wakeFromStandby();

protected:
    void startDrivers();
    void stopDrivers();

    std::vector<Driver*>& drivers_;

    //Reference Manual Page 333
    const extiline_t COMP1_OUTPUT = 21U;
    const extiline_t COMP2_OUTPUT = 22U;
};
 
} //namespace hebi::firmware::hardware
  
/**
 * Beeper_PWM16.h
 * 
 * Implementation of a beeper using PWMD16
*/

#pragma once

#include "Driver.h"
#include <vector>

namespace hebi::firmware::hardware {
 
class Power_Control {
public:
    Power_Control(std::vector<Driver *>& drivers);

    void sleep();
    void clearStandby();
    bool wakeFromStandby();

protected:
    void startDrivers();
    void stopDrivers();

    std::vector<Driver*>& drivers_;
};
 
} //namespace hebi::firmware::hardware
  
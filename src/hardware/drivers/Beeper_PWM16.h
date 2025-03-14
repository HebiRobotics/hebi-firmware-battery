/**
 * Beeper_PWM16.h
 * 
 * Implementation of a beeper using PWMD16
*/

#pragma once

#include "Beeper.h"
#include "Driver.h"

namespace hebi::firmware::hardware {
 
class Beeper_PWM16 : public Beeper, public Driver {
public:
    Beeper_PWM16(uint16_t frequency);

    void startBeep() override;
    void stopBeep() override;
 
    void startDriver();
    void stopDriver();
};
 
} //namespace hebi::firmware::hardware
  
/**
 * Beeper_PWM16.h
 * 
 * Implementation of a beeper using PWMD16
*/

#pragma once

#include "Beeper.h"

namespace hebi::firmware::hardware {
 
class Beeper_PWM16 : public Beeper {
public:
    Beeper_PWM16(uint16_t frequency);

    void startBeep() override;
    void stopBeep() override;
 
};
 
} //namespace hebi::firmware::hardware
  
/**
 * LED_RGB_PWM.h
 * 
 * Implementation of an RGB LED using PWMD1
*/

#pragma once

#include "LED_RGB.h"
#include "Driver.h"

namespace hebi::firmware::hardware {

class LED_RGB_PWM1 : public LED_RGB, public Driver {
public:
    LED_RGB_PWM1();
    
    void startDriver();
    void stopDriver();

protected:
    void colorUpdated() override;

};

} //namespace hebi::firmware::hardware
 
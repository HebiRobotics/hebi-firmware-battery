/**
 * LED_RGB_PWM.h
 * 
 * Implementation of an RGB LED using PWMD1
*/

#include "Beeper_PWM16.h"

extern "C" {
#include <ch.h>
#include <hal.h>
}

static PWMConfig beeper_pwm_config = {
    .frequency = 0,
    .period = 2,
    .callback = NULL,
    .channels = {
        {PWM_OUTPUT_ACTIVE_HIGH, NULL}, //Speaker connected to channel 1
        {PWM_OUTPUT_DISABLED, NULL},
        {PWM_OUTPUT_DISABLED, NULL},
        {PWM_OUTPUT_DISABLED, NULL}
    },
    .cr2 = 0,
    .bdtr = 0,
    .dier = 0
};

namespace hebi::firmware::hardware {

Beeper_PWM16::Beeper_PWM16(uint16_t frequency){
    beeper_pwm_config.frequency = frequency * beeper_pwm_config.period;
    startDriver();
}

void Beeper_PWM16::startBeep(){
    pwmEnableChannel(&PWMD16, 0, 1);
}

void Beeper_PWM16::stopBeep(){
    pwmDisableChannel(&PWMD16, 0);
}
    
void Beeper_PWM16::startDriver(){
    pwmStart(&PWMD16, &beeper_pwm_config);
}

void Beeper_PWM16::stopDriver(){
    pwmStop(&PWMD16);
}

} //namespace hebi::firmware::hardware
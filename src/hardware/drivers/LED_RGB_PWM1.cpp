/**
 * LED_RGB_PWM.h
 * 
 * Implementation of an RGB LED using PWMD1
 */

#include "LED_RGB_PWM1.h"

extern "C" {
#include <ch.h>
#include <hal.h>
}

static PWMConfig rgb_led_pwm_cfg = {
    .frequency = 1000000,
    .period = 255,
    .callback = NULL,
    .channels = {
        {PWM_COMPLEMENTARY_OUTPUT_ACTIVE_LOW, NULL}, //G
        {PWM_COMPLEMENTARY_OUTPUT_ACTIVE_LOW, NULL}, //R
        {PWM_COMPLEMENTARY_OUTPUT_ACTIVE_LOW, NULL}, //B
        {PWM_OUTPUT_DISABLED, NULL}
    },
    .cr2 = 0,
    .bdtr = 0,
    .dier = 0
};

namespace hebi::firmware::hardware {

LED_RGB_PWM1::LED_RGB_PWM1(){
    pwmStart(&PWMD1, &rgb_led_pwm_cfg);
}

void LED_RGB_PWM1::colorUpdated(){
    pwmEnableChannel(&PWMD1, 0, g_);
    pwmEnableChannel(&PWMD1, 1, r_);
    pwmEnableChannel(&PWMD1, 2, b_);
}
   
    
} //namespace hebi::firmware::hardware
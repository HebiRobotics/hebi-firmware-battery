/**
 * Battery_Node_CAN.cpp
 * 
 * 
*/

#include "power_control.h"

extern "C" {
#include <ch.h>
#include <hal.h>
}

namespace hebi::firmware::hardware {

static void button_callback(void *arg){
    (void) arg;


}

Power_Control::Power_Control(std::vector<Driver *>& drivers) : 
    drivers_(drivers) {
    
    // palEnableLineEvent(LINE_PB_WKUP, PAL_EVENT_MODE_FALLING_EDGE);
    // palSetLineCallback(LINE_PB_WKUP, button_callback, NULL);
}
    
void Power_Control::startDrivers(){
    for (auto driver : drivers_){
        if(driver) //We shouldn't ever get a NULL here... but just in case
            driver->startDriver();
    }
}

void Power_Control::stopDrivers(){
    for (auto driver : drivers_){
        if(driver) //We shouldn't ever get a NULL here... but just in case
            driver->stopDriver();
    }
}

bool Power_Control::wakeFromStandby(){
    return (PWR->SR1 & PWR_SR1_SBF);
}

void Power_Control::clearStandby(){
    PWR->SCR |= PWR_SCR_CSBF; //Clear standby flags
    PWR->CR3 &= ~PWR_CR3_APC; //Clear pullup config
}

void Power_Control::sleep(){

    stopDrivers();

    PWR->PUCRA |= (PWR_PUCRA_PA10 | PWR_PUCRA_PA15); //Pullup on CAN control pins
    PWR->PDCRH |= (PWR_PDCRH_PH3);

    PWR->CR3 &= ~PWR_CR3_EWUP1; //Clear wakeup enable
    PWR->SCR |= PWR_SCR_CWUF; //Clear wakeup flags
    PWR->CR3 |= (PWR_CR3_EWUP1 | PWR_CR3_APC); //Enable wakeup pin and pullups

    // PWR->CR4 |= PWR_CR4_WP1; //Set to falling edge wakeup

    // PWR->CR1 |= PWR_CR1_LPR; //Set to low power mode
    PWR->CR1 |= PWR_CR1_LPMS_SHUTDOWN;
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    
    // __disable_irq();
    // __SEV();
    // __WFE();
    __WFI();
    // __enable_irq();

    while(true) {} //Never reaches here
    // startDrivers(); 
}

} //namespace hebi::firmware::hardware
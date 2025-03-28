/**
 * Battery_Node_CAN.cpp
 * 
 * 
*/

#include "power_control.h"


static void button_callback(void* aux){
    //Do nothing
}

namespace hebi::firmware::hardware {


Power_Control::Power_Control(std::vector<Driver *>& drivers) : 
    drivers_(drivers) {
    
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

void Power_Control::enterStop2(){

    stopDrivers();

    PWR->PUCRA |= (PWR_PUCRA_PA10 | PWR_PUCRA_PA15); //Pullup on CAN control pins
    PWR->PDCRH |= (PWR_PDCRH_PH3);

    PWR->CR3 &= ~PWR_CR3_EWUP4; //Clear wakeup enable
    PWR->SCR |= PWR_SCR_CWUF; //Clear wakeup flags
    PWR->CR3 |= (PWR_CR3_EWUP4 | PWR_CR3_APC); //Enable wakeup pin and pullups

    extiEnableLine(COMP1_OUTPUT, EXTI_MODE_RISING_EDGE | EXTI_MODE_ACTION_EVENT); //Enable EXTI for COMP 1

    SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI2_Msk;
    extiEnableLine(2, EXTI_MODE_RISING_EDGE | EXTI_MODE_ACTION_EVENT); //Enable EXTI for PA2

    PWR->CR1 |= PWR_CR1_LPMS_STOP2;
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    
    //----- Wait for Wakeup! -----

    __SEV();
    __WFE();
    __WFE();

    //----- Wakeup triggered! -----

    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk; //Clear SLEEPDEEP
    PWR->CR3 &= ~(PWR_CR3_APC); //Disable pullups

    startDrivers(); 
}


void Power_Control::enterStandby(){
    stopDrivers();

    PWR->PUCRA |= (PWR_PUCRA_PA10 | PWR_PUCRA_PA15); //Pullup on CAN control pins
    PWR->PDCRH |= (PWR_PDCRH_PH3);

    PWR->CR3 &= ~PWR_CR3_EWUP4; //Clear wakeup enable
    PWR->SCR |= PWR_SCR_CWUF; //Clear wakeup flags
    PWR->CR3 |= (PWR_CR3_EWUP4 | PWR_CR3_APC); //Enable wakeup pin and pullups

    PWR->CR1 |= PWR_CR1_LPMS_SHUTDOWN;
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    
    __WFI();

    while(true) {} //Never reaches here
}

} //namespace hebi::firmware::hardware
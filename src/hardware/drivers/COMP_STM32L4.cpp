/**
 * COMP_STM32L4.cpp
 * 
 * Driver for built STM32L432xx built in comparators
 * 
 * TRM:
 * https://www.st.com/resource/en/reference_manual/rm0394-stm32l41xxx42xxx43xxx44xxx45xxx46xxx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
*/

#include "COMP_STM32L4.h"

extern "C" {
#include <ch.h>
#include <hal.h>
}

namespace hebi::firmware::hardware {

COMP_STM32L4::COMP_STM32L4(){
    startDriver();

}
   
bool COMP_STM32L4::output_comp1(){
    return COMP1->CSR & COMP_CSR_VALUE;
}

bool COMP_STM32L4::output_comp2(){
    return COMP2->CSR & COMP_CSR_VALUE;
}

//TRM Page 541
void COMP_STM32L4::startDriver(){
    uint32_t base_config = 
        COMP_CSR_EN                             | //Enable comparator
        COMP_CSR_PWRMODE                        | //Low power mode
        COMP_CSR_INMSEL_0 | COMP_CSR_INMSEL_1   | //Select V_REFINT for minus input
        COMP_CSR_SCALEN;                          //Enable bandgap


    // rccEnableAPB2(RCC_APB2ENR_SYSCFGEN, true);
    COMP1->CSR |= (base_config | COMP_CSR_INPSEL_1);    //TRM Page 532, PA1 = 0b10
    COMP2->CSR |= (base_config);                        //TRM Page 532, PB4 = 0b00
}

void COMP_STM32L4::stopDriver(){
    // COMP1->CSR &= ~COMP_CSR_EN; //Comp 1 is used for wakeup... do not disable!
    COMP2->CSR &= ~COMP_CSR_EN;
}

} //namespace hebi::firmware::hardware
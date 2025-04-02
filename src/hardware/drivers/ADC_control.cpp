/**
 * ADC_control.cpp
 * 
 * Driver for ADC with low power shutdown and STM32L4 internal OP-AMP
 * 
 * TRM:
 * https://www.st.com/resource/en/reference_manual/rm0394-stm32l41xxx42xxx43xxx44xxx45xxx46xxx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
*/

#include "ADC_control.h"

extern "C" {
#include <ch.h>
#include <hal.h>
}

namespace hebi::firmware::hardware {

/* Buffers are allocated with size and address aligned to the cache
   line size.*/
#define ADC_BUF_DEPTH       2
#define ADC_NUM_CHANNELS    2
#define ADC_MAX             (8.f * 4096.f) //8x oversampled 12 bits
#define ADC_CONV_SCALE_BAT  ((3.3f / ADC_MAX) * (33.f + 2.f) / (2.f))
#define ADC_CONV_SCALE_EXT  ((3.3f / ADC_MAX) * (84.5f + 2.55f) / (2.55f))

static adcsample_t samples[CACHE_SIZE_ALIGN(adcsample_t, ADC_NUM_CHANNELS * ADC_BUF_DEPTH)];
uint8_t ADC_control::v_bat_which_index_ {0};
uint8_t ADC_control::v_ext_which_index_ {0};

void adccallback(ADCDriver *adcp) {
    ADC_control::dataUpdated(adcIsBufferComplete(adcp));
}

static const ADCConversionGroup adccfg = {
    .circular     = true,
    .num_channels = ADC_NUM_CHANNELS,
    .end_cb       = adccallback,
    .error_cb     = NULL,
    .cfgr         = ADC_CFGR_RES_12BITS | ADC_CFGR_CONT,
    .cfgr2        = ADC_CFGR2_OVSR_1 /* 8X Oversampling */ |
                    ADC_CFGR2_ROVSE /* Oversampling enabled for Regular channels*/,
    .tr1          = ADC_TR_DISABLED,
    .tr2          = ADC_TR_DISABLED,
    .tr3          = ADC_TR_DISABLED,
    .awd2cr       = 0U,
    .awd3cr       = 0U,
    .smpr         = {
        ADC_SMPR1_SMP_AN6(ADC_SMPR_SMP_92P5) | ADC_SMPR1_SMP_AN8(ADC_SMPR_SMP_92P5), 
        0U
    },
    .sqr          = {
        ADC_SQR1_SQ1_N(ADC_CHANNEL_IN8) | ADC_SQR1_SQ2_N(ADC_CHANNEL_IN6),
        0U,
        0U,
        0U
    }
};



ADC_control::ADC_control(){
    startDriver();

}

void ADC_control::dataUpdated(bool which){
    //Change valid index to point at recent data
    uint8_t which_index = (which) ? ADC_NUM_CHANNELS : 0; 
    
    //Each value is in sequential order
    v_bat_which_index_ = which_index;
    v_ext_which_index_ = which_index + 1; 
}

float ADC_control::v_bat(){
    return samples[v_bat_which_index_] * ADC_CONV_SCALE_BAT;
}

float ADC_control::v_ext(){
    return samples[v_ext_which_index_] * ADC_CONV_SCALE_EXT;
}

//TRM Page 552
void ADC_control::startDriver(){
    palWriteLine(LINE_SNS_EN, PAL_HIGH); //Enable voltage divider

    uint32_t op_amp_cfg = 
        OPAMP_CSR_OPALPM                            | //OP-AMP low power mode
        OPAMP_CSR_OPAMODE_1 | OPAMP_CSR_OPAMODE_0   | //Set to internal follower mode
        OPAMP1_CSR_OPARANGE;                          //VDDA > 2.4V

    rccEnableAPB1R1(RCC_APB1ENR1_OPAMPEN, true);
    OPAMP1->CSR |= op_amp_cfg; //Configure the OPAMP
    OPAMP1->CSR |= OPAMP_CSR_OPAMPxEN; //Enable the OPAMP

    //Start ADC driver
    adcStart(&ADCD1, NULL);
    adcStartConversion(&ADCD1, &adccfg,
        samples, ADC_NUM_CHANNELS * ADC_BUF_DEPTH);
}

void ADC_control::stopDriver(){

    OPAMP1->CSR &= ~OPAMP_CSR_OPAMPxEN; //Disable the OPAMP

    palWriteLine(LINE_SNS_EN, PAL_LOW); //Disable voltage divider

    //Stop ADC Driver
    adcStopConversion(&ADCD1);
    adcStop(&ADCD1);
}

} //namespace hebi::firmware::hardware
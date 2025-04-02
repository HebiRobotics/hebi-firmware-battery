/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

extern "C" {
#include <ch.h>
#include <hal.h>
void __late_init();
}

#include "modules/Beep_Controller.h"
#include "modules/LED_Controller.h"
#include "modules/Pushbutton_Controller.h"
#include "hardware/drivers/LED_RGB_PWM1.h"
#include "hardware/drivers/Beeper_PWM16.h"
#include "hardware/drivers/battery_node_CAN.h"
#include "hardware/drivers/BQ34Z100_I2C.h"
#include "hardware/drivers/Flash_STM32L4.h"
#include "hardware/drivers/power_control.h"
#include "hardware/drivers/COMP_STM32L4.h"
#include "hardware/drivers/ADC_control.h"

#include "battery_node.h"

#include "hardware/Driver.h"
#include <array>
#include <vector>

using namespace hebi::firmware;


const static I2CConfig I2C_BATTERY_CONFIG = {
    0x00000E14, //this is generated using STM32CubeMX
    0,
    0
};

hardware::Beeper_PWM16 beeper_driver(4000 /*4kHz*/);
modules::Beep_Controller beeper (beeper_driver);
hardware::LED_RGB_PWM1 rgb_led_driver;
modules::LED_Controller status_led (rgb_led_driver);
hardware::COMP_STM32L4 comp;
hardware::ADC_control adc;

modules::Pushbutton_Controller button (400 /*ms*/, 600 /*ms*/);

hardware::Flash_STM32L4 database;

hardware::Battery_Node_CAN can;
hardware::BQ34Z100_I2C battery_i2c(&I2CD1, I2C_BATTERY_CONFIG);

std::vector<hardware::Driver *> drivers{&beeper_driver, &rgb_led_driver, &can, &battery_i2c, &comp, &adc};
hardware::Power_Control power_ctrl(drivers);

Battery_Node battery_node(database, status_led, button, can, power_ctrl);

/**
 * @brief Initializes hal and ChibiOS
 *
 * @note This function overrides the definition in crt0.c
 *
 * @note This function is called before any static constructors
 *
 * @return Void
 */
void __late_init() {
    /*
    * System initializations.
    * - HAL (Hardware Abstraction Layer) initialization, this initializes 
    *   the configured device drivers and performs the board-specific 
    *   initializations.
    * - Kernel initialization, the main() function becomes a thread and the
    *   RTOS is active.
    */
    halInit();
    chSysInit();
}

/*
 * Application entry point.
 */
int main(void) {

    if(!power_ctrl.wakeFromStandby()){
        power_ctrl.enterStop2();
    } else {
        power_ctrl.clearStandby();
    }

    rgb_led_driver.setColor(255,0,0);
    static uint16_t count = 0;

    while (true) {

        volatile float voltage = adc.v_bat();
        volatile float voltage_ext = adc.v_ext();

        button.update(palReadLine(LINE_PB_WKUP));

        if(button.enabled()){
            if(button.stateChanged()){
                beeper.beepTwice();
            }
        } else {
            if(button.stateChanged()){
                beeper.beepOnce(400);
            }
        }

        battery_node.update(comp.output_comp1(), comp.output_comp2());

        status_led.update();
        beeper.update();

        palWriteLine(LINE_DSG_EN, battery_node.dsgEnable());
        palWriteLine(LINE_CHG_EN, battery_node.chgEnable());

        if(battery_i2c.hasData() && battery_node.shouldSendBatteryData()){
            auto data = battery_i2c.getData();
            protocol::battery_state_msg msg(battery_node.nodeID(), data.voltage, data.current, data.capacity_remaining, data.capacity_full);
            can.sendMessage(msg);
        }

        chThdSleepMilliseconds(1);
    }
}

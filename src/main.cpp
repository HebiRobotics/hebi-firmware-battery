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

#include "modules/LED_Controller.h"
#include "hardware/drivers/LED_RGB_PWM1.h"

using namespace hebi::firmware;

// #include "rt_test_root.h"
// #include "oslib_test_root.h"

/*
 * Green LED blinker thread, times are in milliseconds.
 */
// static THD_WORKING_AREA(waThread1, 128);
// static THD_FUNCTION(Thread1, arg) {

//   (void)arg;
//   chRegSetThreadName("blinker");
//   while (true) {
//     palClearLine(LINE_LED_G);
//     chThdSleepMilliseconds(500);
//     palSetLine(LINE_LED_G);
//     chThdSleepMilliseconds(500);
//   }
// }


hardware::LED_RGB_PWM1 rgb_led_driver;
modules::LED_Controller status_led (rgb_led_driver);

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

    status_led.green().fade();

    while (true) {
        status_led.update();

        chThdSleepMilliseconds(1);
    }
}

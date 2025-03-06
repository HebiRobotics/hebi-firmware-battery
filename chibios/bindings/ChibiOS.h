/*
 * @file seasnake.h
 *
 * @brief Utility macros and ChibiOS includes necessary for operation of
 * SEA snake test code.
 *
 * Created on: May 30, 2013
 * 
 * @author Pras Velagapudi (pkv@cs)
 * @author Yigit Bilgen (mbilgen@andrew)
 */

#ifndef CHIBI_OS_H_
#define CHIBI_OS_H_

#include <ch.hpp>

extern "C" {
#include <hal.h>
}

namespace chibios_utils {
/**
 * @brief Disables *ALL* interrupts.
 *
 * Interrupts should be masked before calling this function.
 * This deactivates all bootloader functionality and gets called 
 * when the bootloader loads the application. NEVER CALL IT ELSEWHERE!
 *
 * @return Void
 */
#pragma GCC diagnostic ignored "-Wunused-function"

static void disable_all_interrupts() {
    // Disable all NVIC (ARM) interrupts

    for (int i = 0; i < 8; i++) {
      NVIC->ICER[i] = 0xFFFFFFFF;
    }

    for (int i = 0; i < 240; i++) {
      NVIC->IP[i] = 0;
    }

    // Disable systick, it's separate from NVIC
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk;
}
} /* chibios_utils */

#endif /* CHIBI_OS_SETUP_H_ */

/*
 * stubs.c
 *
 *  Created on: Jul 10, 2011
 *      Author: mabl
 */

#include <sys/stat.h>

#include "ch.h"
#include "hal.h"

#include "stubs.h"


int __cxa_guard_acquire(__guard *g) {
  chSysLock();
  const int result = !*(char *)(g);
  chSysUnlock();

  return result;
}

void __cxa_guard_release(__guard *g) {
  chSysLock();
  *(char *)g = 1;
  chSysUnlock();
}

void __cxa_guard_abort(__guard *g) {
  chSysLock();
  *(char *)g = 1;
  chSysUnlock();
}

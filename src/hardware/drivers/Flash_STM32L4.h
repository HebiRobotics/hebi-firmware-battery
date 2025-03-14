/**
 * Flash_STM32L4.h
 * 
 * Flash storage for STM32L4 using ChibiOS MFS driver
*/

#include "Flash_Database.h"
#include "Driver.h"

#pragma once

namespace hebi::firmware::hardware {

class Flash_STM32L4 : public Flash_Database, public Driver {
public:
    Flash_STM32L4();

    void startDriver();
    void stopDriver();

protected:
    bool getArray(FlashDatabaseKey key, uint8_t *data, uint32_t size) const override;
    bool putArray(FlashDatabaseKey key, const uint8_t *data, uint32_t size) override;

};

} //namespace hebi::firmware::hardware
 
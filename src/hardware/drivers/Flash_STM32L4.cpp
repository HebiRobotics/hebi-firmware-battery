#include "Flash_STM32L4.h"

extern "C" {
#include <ch.h>
#include <hal.h>
#include <hal_mfs.h>
}

namespace hebi::firmware::hardware {


mfs_nocache_buffer_t __nocache_mfsbuf1;
MFSDriver mfs1;
uint8_t __nocache_mfs_buffer[512];

const MFSConfig mfscfg1 = {
    .flashp           = (BaseFlash *)&EFLD1,
    .erased           = 0xFFFFFFFFU,
    .bank_size        = 4096U,
    .bank0_start      = 122U,
    .bank0_sectors    = 2U,
    .bank1_start      = 124U,
    .bank1_sectors    = 2U
};


Flash_STM32L4::Flash_STM32L4(){

    /* 
    Starting EFL driver.
    No config needed.
    */
    eflStart(&EFLD1, NULL);

    mfsObjectInit(&mfs1, &__nocache_mfsbuf1);
    mfsStart(&mfs1, &mfscfg1);

}

bool Flash_STM32L4::getArray(FlashDatabaseKey key, uint8_t *data, uint32_t size) const {
    mfs_id_t id = static_cast<mfs_id_t>(key);
    size_t size_ = size;
    mfs_error_t err = mfsReadRecord(&mfs1, id, &size_, data);

    return size_ == size && err == MFS_NO_ERROR;
}

bool Flash_STM32L4::putArray(FlashDatabaseKey key, const uint8_t *data, uint32_t size) {
    mfs_id_t id = static_cast<mfs_id_t>(key);
    mfs_error_t err = mfsWriteRecord(&mfs1, id, size, data);

    return err == MFS_NO_ERROR;
}

} //namespace hebi::firmware::hardware
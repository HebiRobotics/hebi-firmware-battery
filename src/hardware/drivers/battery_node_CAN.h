/**
 * Battery_Node_CAN.h
 * 
 * 
*/

#pragma once

#include "all_msg.h"
#include "Driver.h"
#include "driver/LockFreeRingBuffer.h"

extern "C" {
#include <ch.h>
#include <hal.h>
}
    

namespace hebi::firmware::hardware {

class Battery_Node_CAN : public Driver {
public:
    Battery_Node_CAN();

    void sendMessage(protocol::base_msg msg);
    std::optional<protocol::base_msg> getMessage() { return rx_buffer_.take(); }

    void startDriver() override;
    void stopDriver() override;
protected:
    static THD_FUNCTION(can_tx_thd_fn, arg);
    static THD_WORKING_AREA(can_tx_wa, 256);
    static thread_t *can_tx_thread_;

    static THD_FUNCTION(can_rx_thd_fn, arg);
    static THD_WORKING_AREA(can_rx1_wa, 256);
    static thread_t *can_rx_thread_;
    
    static util::LF_RingBuffer<protocol::base_msg, 5> rx_buffer_;
    static util::LF_RingBuffer<protocol::base_msg, 5> tx_buffer_;

    static CANConfig cancfg_;
};

} //namespace hebi::firmware::hardware
 
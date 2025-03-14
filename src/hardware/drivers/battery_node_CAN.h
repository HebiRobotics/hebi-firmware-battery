/**
 * Battery_Node_CAN.h
 * 
 * 
*/

#pragma once

#include "all_msg.h"
#include "battery_node.h"
#include "Driver.h"

extern "C" {
#include <ch.h>
#include <hal.h>
}
    

namespace hebi::firmware::hardware {

class Battery_Node_CAN : public Driver {
public:
    Battery_Node_CAN(Battery_Node& can_node);

    void sendMessage(protocol::base_msg msg);

    void startDriver();
    void stopDriver();

    Battery_Node& can_node_;
};

} //namespace hebi::firmware::hardware
 
/**
 * battery_CAN.h
 * 
 * 
*/

#pragma once

#include "all_msg.h"
#include "driver/parser.h"

extern "C" {
#include <ch.h>
#include <hal.h>
}
    

namespace hebi::firmware::hardware {

class Battery_CAN : public protocol::Message_Parser {
public:
    Battery_CAN();

    void sendMessage(protocol::base_msg msg);
};

} //namespace hebi::firmware::hardware
 
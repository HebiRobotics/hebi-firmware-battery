/**
 * battery_CAN.h
 * 
 * 
*/

#pragma once

#include "all_msg.h"
#include "battery_node.h"

extern "C" {
#include <ch.h>
#include <hal.h>
}
    

namespace hebi::firmware::hardware {

class Battery_CAN : public Battery_Node {
public:
    Battery_CAN(hardware::Flash_Database& database, modules::LED_Controller& led, 
        modules::Pushbutton_Controller& button_ctrl);

    void sendMessage(protocol::base_msg msg);
};

} //namespace hebi::firmware::hardware
 
/**
 * battery_CAN.h
 * 
 * 
*/

#pragma once

#include "all_msg.h"


namespace hebi::firmware::hardware {

class Battery_CAN {
public:
    Battery_CAN();

    void sendMessage(protocol::base_msg msg);

};

} //namespace hebi::firmware::hardware
 
/* Battery_Node.h
    
*/

#pragma once

#include "can-proto/driver/base_node.h"
#include "hardware/Flash_Database.h"
#include "modules/LED_Controller.h"
#include "modules/Pushbutton_Controller.h"

namespace hebi::firmware {

class Battery_Node : public protocol::Base_Node {
public:
    Battery_Node(hardware::Flash_Database& database, 
        modules::LED_Controller& led, 
        modules::Pushbutton_Controller& button_ctrl);

    void update() override;
    
    void addTxMessage(protocol::base_msg msg) override {
        if(!node_id_valid_) return;

        Base_Node::addTxMessage(msg);
    }

    bool shouldSendBatteryData(){
        return send_battery_data_;
    }

    uint8_t nodeID(){
        return node_id_;
    }

protected:
    void initNodeID();

    void recvd_ctrl_poll_node_id(protocol::ctrl_poll_node_id_msg msg) override;
    void recvd_ctrl_set_node_id(protocol::ctrl_set_node_id_msg msg) override;

    void recvd_cmd_start_data(protocol::cmd_start_data_msg msg) override;
    void recvd_cmd_set_led(protocol::cmd_set_led_msg msg) override;
    void recvd_cmd_disable_output(protocol::cmd_disable_output_msg msg) override;
    void recvd_cmd_enable_output(protocol::cmd_enable_output_msg msg) override;
    
    bool send_battery_data_ { false };
    bool node_id_valid_ { false };
    uint8_t node_id_ { protocol::DEFAULT_NODE_ID };

    hardware::Flash_Database& database_;
    modules::LED_Controller& led_;
    modules::Pushbutton_Controller& button_ctrl_;
    
    // uint8_t counter_ {0};
};

};
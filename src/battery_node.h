/* Battery_Node.h
    
*/

#pragma once

#include "hardware/drivers/battery_node_CAN.h"
#include "can-proto/driver/base_node.h"
#include "hardware/Flash_Database.h"
#include "modules/LED_Controller.h"
#include "modules/Pushbutton_Controller.h"
#include "hardware/drivers/power_control.h"
#include "modules/Beep_Controller.h"

namespace hebi::firmware {

class Battery_Node : public protocol::Base_Node {
    enum class NodeState {
        INIT = 0,                   //Startup Value
        LOW_POWER_TIMEOUT = 1,      //Waiting to shutdown
        LOW_POWER_SHUTDOWN = 2,     //Shutdown, waiting for wakeup
        LOW_POWER_DEEP_SLEEP = 3,   //Lower power shutdown (TODO)
        FAULT_SILENT = 4,           //Fault detected, but no user feedback (low power mode)
        FAULT = 5,                  //Fault detected with user feedback
        OUTPUT_ENABLED = 6,         //Normal operation, DSG / CHG enabled
        CHARGE_ENABLED = 7,         //Charger detected, DSG disabled, CHG enabled
        CHARGE_TIMEOUT = 8,         //Charger detected but timeout reached, DSG & CHG disabled

        //CAN Special states - Outputs off
        ID_ACQUISITION_WAIT = 20,   //Waiting for CAN node id assignment.
        ID_ACQUISITION_TAKE = 21,   //ID acquisition triggered, take next available ID.
        ID_ACQUISITION_DONE = 22,   //ID valid. Wait for acquisition to end.
    };



public:
    Battery_Node(hardware::Flash_Database& database, 
        modules::LED_Controller& led, 
        modules::Pushbutton_Controller& button_ctrl,
        modules::Beep_Controller& beeper,
        protocol::CAN_driver& can_driver,
        hardware::Power_Control& power_ctrl);

    void update(bool chg_detect, bool polarity_ok);

    bool chgEnable() { return chg_enable_; }
    bool dsgEnable() { return dsg_enable_; }
    bool isFaulted() { return state_ == NodeState::FAULT; }
    
    void addTxMessage(protocol::base_msg msg) {
        if(!node_id_valid_) return;

        can_driver_.sendMessage(msg);
    }

    bool shouldSendBatteryData(){
        return send_battery_data_;
    }

    uint8_t nodeID(){
        return node_id_;
    }

protected:
    static const uint32_t LOW_POWER_TIMEOUT_MS = 2000;
    static const uint32_t CHARGE_TIMEOUT_MS = 20 * 60 * 1000;

    static const uint16_t FAULT_CODE_REVERSE_POLARITY = 0x01;

    void initNodeID();
    void changeNodeState(NodeState state);
    void changeNodeStateFromISR(NodeState state);
    void changeNodeStateUnsafe(NodeState state);
    void enterLowPowerMode();
    void enterFaultState(uint16_t fault_code);

    void recvd_ctrl_poll_node_id(protocol::ctrl_poll_node_id_msg msg) override;
    void recvd_ctrl_set_node_id(protocol::ctrl_set_node_id_msg msg) override;
    void recvd_ctrl_start_acquisition(protocol::ctrl_start_acquisition_msg msg) override; 
    void recvd_ctrl_stop_acquisition(protocol::ctrl_stop_acquisition_msg msg) override; 

    void recvd_cmd_start_data(protocol::cmd_start_data_msg msg) override;
    void recvd_cmd_set_led(protocol::cmd_set_led_msg msg) override;
    void recvd_cmd_disable_output(protocol::cmd_disable_output_msg msg) override;
    void recvd_cmd_enable_output(protocol::cmd_enable_output_msg msg) override;
    
    bool dsg_enable_ { false };
    bool chg_enable_ { false };

    bool send_battery_data_ { false };
    bool node_id_valid_ { false };
    uint8_t node_id_ { protocol::DEFAULT_NODE_ID };
    NodeState state_ { NodeState::INIT };
    uint32_t state_counter_ {0};
    uint16_t fault_code_ {0};

    hardware::Flash_Database& database_;
    modules::LED_Controller& led_;
    modules::Beep_Controller& beeper_;
    modules::Pushbutton_Controller& button_;
    protocol::CAN_driver& can_driver_;
    hardware::Power_Control& power_ctrl_;
    
    // uint8_t counter_ {0};
};

};
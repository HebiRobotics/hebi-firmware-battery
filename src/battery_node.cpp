#include "battery_node.h"

extern "C" {
    #include <ch.h>
    #include <hal.h>
}

using namespace hebi::firmware;

Battery_Node::Battery_Node(hardware::Flash_Database& database, 
    modules::LED_Controller& led, 
    modules::Pushbutton_Controller& button_ctrl,
    hardware::Battery_Node_CAN& can_driver,
    hardware::Power_Control& power_ctrl) :
    database_(database), led_(led), button_(button_ctrl), 
    can_driver_(can_driver), power_ctrl_(power_ctrl) {
    
    initNodeID();
}

void Battery_Node::initNodeID(){
    node_id_valid_ = database_.get(hardware::FlashDatabaseKey::NODE_ID, node_id_);
    if(!node_id_valid_)
        node_id_ = protocol::DEFAULT_NODE_ID;
}

void Battery_Node::update(bool chg_detect, bool polarity_ok) {
    bool has_data = true;
    bool msg_recvd = false;

    while(has_data){
        auto msg_opt = can_driver_.getMessage();
        if(msg_opt.has_value()){
            msg_recvd |= tryParseMsg(msg_opt.value());
        } else {
            has_data = false;
        }

    }

    switch(state_){
    case NodeState::LOW_POWER_TIMEOUT:
        state_counter_++;
        if(msg_recvd) //Reset timeout when we receive a CAN message
            state_counter_ = 0;

        if(!polarity_ok) { //Reverse polarity fault
            enterFaultState(FAULT_CODE_REVERSE_POLARITY);
        } else if(button_.enabled()){
            changeNodeState(NodeState::OUTPUT_ENABLED);
        } else if(chg_detect) {
            //TODO: Check voltage with ADC
            changeNodeState(NodeState::CHARGE_ENABLED);
        } else if(state_counter_ == LOW_POWER_TIMEOUT_MS){
            enterLowPowerMode();
        }
        break;
    case NodeState::FAULT_SILENT:
        //TODO: Check battery state, Reset behavior
        if(button_.enabled()){ 
            changeNodeState(NodeState::FAULT);
        }
        break;
    case NodeState::OUTPUT_ENABLED:
    case NodeState::FAULT:
        if(!button_.enabled()){ 
            changeNodeState(NodeState::LOW_POWER_TIMEOUT);
        }
        break;
    case NodeState::CHARGE_ENABLED:
        state_counter_++;

        if(button_.enabled()){ 
            changeNodeState(NodeState::OUTPUT_ENABLED);
        } else if(!chg_detect) {
            changeNodeState(NodeState::LOW_POWER_TIMEOUT);
        } else if (state_counter_ == CHARGE_TIMEOUT_MS) {
            changeNodeState(NodeState::CHARGE_TIMEOUT);
        }
        break;
    case NodeState::CHARGE_TIMEOUT:
        if(button_.enabled()){ 
            changeNodeState(NodeState::OUTPUT_ENABLED);
        } else if(!chg_detect) {
            changeNodeState(NodeState::LOW_POWER_TIMEOUT);
        }
        break;
    default:
        //Do nothing!
        break;
    }
}

void Battery_Node::changeNodeState(NodeState state){
    chSysLock();
    changeNodeStateUnsafe(state);
    chSysUnlock();
}

void Battery_Node::changeNodeStateFromISR(NodeState state){
    chSysLockFromISR();
    changeNodeStateUnsafe(state);
    chSysUnlockFromISR();
}

void Battery_Node::enterLowPowerMode(){
    changeNodeState(NodeState::LOW_POWER_SHUTDOWN);

    power_ctrl_.enterStop2();

    //We will resume from here upon wakeup from stop2
    changeNodeState(NodeState::LOW_POWER_TIMEOUT);
}

void Battery_Node::enterFaultState(uint16_t fault_code){
    changeNodeState(NodeState::FAULT_SILENT);
    fault_code_ = fault_code;
}

void Battery_Node::changeNodeStateUnsafe(NodeState state){
    switch (state){
    case NodeState::LOW_POWER_TIMEOUT: {
        //Can enter from any state
        dsg_enable_ = false;
        chg_enable_ = false;
        led_.off();

        state_counter_ = 0;
        state_ = NodeState::LOW_POWER_TIMEOUT;
        break;
    }
    case NodeState::LOW_POWER_SHUTDOWN: {
        if(state_ != NodeState::LOW_POWER_TIMEOUT) return;

        dsg_enable_ = false;
        chg_enable_ = false;
        led_.off();

        state_ = NodeState::LOW_POWER_SHUTDOWN;
        break;
    }
    /*case NodeState::LOW_POWER_DEEP_SLEEP: {
        //TODO: Implement this state
        break;
    }*/
    case NodeState::FAULT_SILENT: {
        
        dsg_enable_ = false;
        chg_enable_ = false;
        led_.off();

        state_ = NodeState::FAULT_SILENT;
        break;
    }
    case NodeState::FAULT: {

        dsg_enable_ = false;
        chg_enable_ = false;
        led_.red().blinkFast();

        state_ = NodeState::FAULT;
        break;
    }
    case NodeState::OUTPUT_ENABLED: {
        dsg_enable_ = true;
        chg_enable_ = true;
        led_.green().fade();

        state_ = NodeState::OUTPUT_ENABLED;
        break;
    }
    case NodeState::CHARGE_ENABLED: {
        dsg_enable_ = false;
        chg_enable_ = true;
        led_.orange().blink();

        state_counter_ = 0;
        state_ = NodeState::CHARGE_ENABLED;
        break;
    }
    case NodeState::CHARGE_TIMEOUT: {
        dsg_enable_ = false;
        chg_enable_ = false;
        led_.green().blink();

        state_ = NodeState::CHARGE_TIMEOUT;
        break;
    }
    /* CAN Special States */
    case NodeState::ID_ACQUISITION_WAIT: {
        if(state_ != NodeState::LOW_POWER_TIMEOUT) return;

        dsg_enable_ = false;
        chg_enable_ = false;
        led_.blue().orange().blink();

        state_ = NodeState::ID_ACQUISITION_WAIT;
        break;
    }
    case NodeState::ID_ACQUISITION_TAKE: {
        if(state_ != NodeState::ID_ACQUISITION_WAIT) return;

        dsg_enable_ = false;
        chg_enable_ = false;
        led_.blue().orange().blinkFast();

        state_ = NodeState::ID_ACQUISITION_TAKE;
        break;
    }
    case NodeState::ID_ACQUISITION_DONE: {
        if(state_ != NodeState::ID_ACQUISITION_TAKE || 
            state_ != NodeState::LOW_POWER_TIMEOUT) return;

        dsg_enable_ = false;
        chg_enable_ = false;
        led_.blue().fade();

        state_ = NodeState::ID_ACQUISITION_DONE;
        break;
    }
    default:
        //Invalid, do nothing
        break;
    }
}

void Battery_Node::recvd_ctrl_start_acquisition(protocol::ctrl_start_acquisition_msg msg){
    //Request must be coming from master node
    if(msg.EID.node_id != 0x00) return; 

    //Go to off state first to ensure proper transition
    changeNodeState(NodeState::LOW_POWER_TIMEOUT);

    /*  If "should_clear_id" is true, every node receiving this message should 
        reset its ID and start the acquisition process.
    */
    if(msg.should_clear_id() && node_id_ != protocol::DEFAULT_NODE_ID) {
        node_id_ = protocol::DEFAULT_NODE_ID;
        database_.put(hardware::FlashDatabaseKey::NODE_ID, node_id_);

        initNodeID();
    }

    /* If we have a valid ID already, skip the acquisition process */
    if(node_id_ == protocol::DEFAULT_NODE_ID)
        changeNodeState(NodeState::ID_ACQUISITION_WAIT);
    else
        changeNodeState(NodeState::ID_ACQUISITION_DONE);
}

void Battery_Node::recvd_ctrl_stop_acquisition(protocol::ctrl_stop_acquisition_msg msg){
    //Request must be coming from master node
    if(msg.EID.node_id != 0x00) return; 

    //Return to off state
    if(state_ == NodeState::ID_ACQUISITION_WAIT || state_ == NodeState::ID_ACQUISITION_DONE)
        changeNodeState(NodeState::LOW_POWER_TIMEOUT);
}

void Battery_Node::recvd_cmd_start_data(protocol::cmd_start_data_msg msg){
    //Request must be coming from master node
    if(msg.EID.node_id != 0x00 && node_id_ != protocol::DEFAULT_NODE_ID) return; 

    send_battery_data_ = true; 
}

void Battery_Node::recvd_ctrl_poll_node_id(protocol::ctrl_poll_node_id_msg msg){
    //Request must be coming from master node
    if(msg.EID.node_id != 0x00 && node_id_ != protocol::DEFAULT_NODE_ID) return; 

    addTxMessage(protocol::ctrl_poll_node_id_msg(node_id_));
}

void Battery_Node::recvd_ctrl_set_node_id(protocol::ctrl_set_node_id_msg msg){
    if(msg.EID.node_id != node_id_) return;

    uint8_t new_node_id = msg.new_node_id();
    if(new_node_id != protocol::DEFAULT_NODE_ID && new_node_id != node_id_){
        database_.put(hardware::FlashDatabaseKey::NODE_ID, new_node_id);

        initNodeID();
    }
}

void Battery_Node::recvd_cmd_set_led(protocol::cmd_set_led_msg msg){
    if(msg.EID.node_id != node_id_) return;

    if(msg.enabled())
        led_.enableOverride(msg.R(), msg.G(), msg.B());
    else
        led_.disableOverride();

}

void Battery_Node::recvd_cmd_disable_output(protocol::cmd_disable_output_msg msg){
    if(msg.EID.node_id != node_id_) return;
    
    button_.forceDisabled();
}

void Battery_Node::recvd_cmd_enable_output(protocol::cmd_enable_output_msg msg){
    if(msg.EID.node_id != node_id_) return;
    
    button_.forceEnabled();

}
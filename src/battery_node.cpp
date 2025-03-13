#include "battery_node.h"

using namespace hebi::firmware;

Battery_Node::Battery_Node(hardware::Flash_Database& database, 
    modules::LED_Controller& led, modules::Pushbutton_Controller& button_ctrl) :
    database_(database), led_(led), button_ctrl_(button_ctrl) {
    
    initNodeID();
}

void Battery_Node::initNodeID(){
    node_id_valid_ = database_.get(hardware::FlashDatabaseKey::NODE_ID, node_id_);
    if(!node_id_valid_)
        node_id_ = protocol::DEFAULT_NODE_ID;
}

void Battery_Node::update() {
    Base_Node::update();
    
}

void Battery_Node::recvd_ctrl_set_node_addr(protocol::ctrl_set_node_id_msg msg){
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
        led_.enableOverride(msg.R(), msg.B(), msg.G());
    else
        led_.disableOverride();

}

void Battery_Node::recvd_cmd_disable_output(protocol::cmd_disable_output_msg msg){
    if(msg.EID.node_id != node_id_) return;
    
    button_ctrl_.forceDisabled();
}

void Battery_Node::recvd_cmd_enable_output(protocol::cmd_enable_output_msg msg){
    if(msg.EID.node_id != node_id_) return;
    
    button_ctrl_.forceEnabled();

}
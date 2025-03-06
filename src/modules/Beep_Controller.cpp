#include "Beep_Controller.h"

using namespace hebi::firmware::modules;

Beep_Controller::Beep_Controller(hardware::Beeper& beeper_driver) :
    beeper_driver_(beeper_driver) {
    //Do nothing
}

void Beep_Controller::update() {
    if(code_index_ < code_length_){
        counter_++;
        if(counter_ >= beep_code_[code_index_]){
            counter_ = 0;
            code_index_++;

            if(code_index_ == code_length_) return;
        }

        //Codes should always be an even numbered length
        //Always format of on time followed by an off time
        if(code_index_ & 1)
            beeper_driver_.stopBeep();
        else
            beeper_driver_.startBeep();
        
    } //else beeping is inactive
}

//TODO: There is probably a better way of writing this function...
void Beep_Controller::setCode(  
    uint16_t on_1, uint16_t off_1, 
    uint16_t on_2, uint16_t off_2, 
    uint16_t on_3, uint16_t off_3, 
    uint16_t on_4, uint16_t off_4, 
    uint16_t on_5, uint16_t off_5 ){
    
    code_index_ = 0;
    code_length_ = 0;
    counter_ = 0;

    if(on_1 != 0){
        beep_code_[0] = trim(on_1);
        beep_code_[1] = trim(off_1);
        code_length_ += 2;
    } else {
        return;
    }
    
    if(on_2 != 0){
        beep_code_[2] = trim(on_2);
        beep_code_[3] = trim(off_2);
        code_length_ += 2;
    } else {
        return;
    }
    
    if(on_3 != 0){
        beep_code_[4] = trim(on_3);
        beep_code_[5] = trim(off_3);
        code_length_ += 2;
    } else {
        return;
    }
    
    if(on_4 != 0){
        beep_code_[6] = trim(on_4);
        beep_code_[7] = trim(off_4);
        code_length_ += 2;
    } else {
        return;
    }
    
    if(on_5 != 0){
        beep_code_[8] = trim(on_5);
        beep_code_[9] = trim(off_5);
        code_length_ += 2;
    } else {
        return;
    }
}

//Avoid 0 or UINT16_MAX
uint16_t Beep_Controller::trim(uint16_t time){
    time = (time == T_MIN) ? T_MIN + 1 : time;
    time = (time == T_MAX) ? T_MAX - 1 : time;
    return time;
}
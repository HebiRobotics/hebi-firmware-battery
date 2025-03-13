/**
 * Pushbutton_Controller.h
 * 
 * Simple interface class for a single-frequency speaker
*/

#pragma once

#include <stdint.h>

namespace hebi::firmware::modules {

class Pushbutton_Controller {
public:
    Pushbutton_Controller(uint16_t t_rising, uint16_t t_falling) :
        t_rising_(t_rising), t_falling_(t_falling) {

    }

    void update(bool button_input){
        switch(button_state_){
        case ButtonState::DISABLED:
            if(button_input)
                count_++;
            else
                count_ = 0;

            if(count_ == t_rising_){
                button_state_ = ButtonState::ENABLED_LOCKED;
                state_changed_ = true;
            }
            break;
        case ButtonState::ENABLED_LOCKED:
            if(!button_input)
                button_state_ = ButtonState::ENABLED;

            count_ = 0;
            break;
        case ButtonState::ENABLED:
            if(button_input)
                count_++;
            else
                count_ = 0;
            
            if(count_ == t_falling_){
                button_state_ = ButtonState::DISABLED_LOCKED;
                state_changed_ = true;
            }
            break;
        case ButtonState::DISABLED_LOCKED:
            if(!button_input)
                button_state_ = ButtonState::DISABLED;

            count_ = 0;
            break;

        }
    }

    bool enabled() {
        return button_state_ == ButtonState::ENABLED_LOCKED || button_state_ == ButtonState::ENABLED;
    }

    bool stateChanged() { //Clear on read flag
        if(state_changed_){
            state_changed_ = false;
            return true;
        }
        return false;
    }

    void forceEnabled(){
        button_state_ = ButtonState::ENABLED_LOCKED;
    }

    void forceDisabled(){
        button_state_ = ButtonState::DISABLED_LOCKED;
    }

private:
    enum class ButtonState{
        DISABLED = 0,
        DISABLED_LOCKED,
        ENABLED,
        ENABLED_LOCKED,
    };

    const uint16_t t_rising_;
    const uint16_t t_falling_;

    uint16_t count_ {0};
    ButtonState button_state_{ButtonState::DISABLED};
    bool state_changed_ {false};

};
 
 } //namespace hebi::firmware::hardware
 
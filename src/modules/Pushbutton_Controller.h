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
    static constexpr bool EDGE_DETECT_MODE = true;
    static constexpr bool TOGGLE_MODE = false;

    Pushbutton_Controller(uint16_t t_rising, uint16_t t_falling, uint16_t t_rising_edge) :
        t_rising_(t_rising), t_falling_(t_falling), t_rising_edge_det_(t_rising_edge) {

    }

    void update(bool button_input){
        switch(button_state_){
        case ButtonState::DISABLED:
            if(button_input)
                count_++;
            else
                count_ = 0;

            if( (count_ == t_rising_ && !edge_detect_mode_) || 
                (count_ == t_rising_edge_det_ && edge_detect_mode_)){

                button_state_ = ButtonState::ENABLED_LOCKED;
                state_changed_ = true;
            }
            break;
        case ButtonState::ENABLED_LOCKED:
            if(!button_input)
                button_state_ = (edge_detect_mode_) ? ButtonState::DISABLED : ButtonState::ENABLED;

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

    void setMode(bool mode) { edge_detect_mode_ = mode; }

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
    const uint16_t t_rising_edge_det_;

    bool edge_detect_mode_ { TOGGLE_MODE };
    uint16_t count_ {0};
    ButtonState button_state_{ButtonState::DISABLED};
    bool state_changed_ {false};

};
 
 } //namespace hebi::firmware::hardware
 
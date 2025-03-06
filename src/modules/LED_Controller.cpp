/**
 * @file LED_Controller.cpp
 * @brief Higher level controller for the onboard LEDs.
 *
 * Created: 28 May 2014
 * 
 * @author Oscar Bezi (bezi@cmu.edu), Matthew Tesch (mtesch@cmu.edu)
 */

#include "LED_Controller.h"

using namespace hebi::firmware::modules;

LED_Controller::LED_Controller(hardware::LED_RGB& led_driver) :
led_driver_(led_driver) {
    curr_background_color_.r = 0;
    curr_background_color_.g = 0;
    curr_background_color_.b = 0;
    curr_foreground_color_.r = 0;
    curr_foreground_color_.g = 0;
    curr_foreground_color_.b = 0;
    curr_display_type_ = OFF;
    override_color_.r = 0;
    override_color_.g = 0;
    override_color_.b = 0;
    is_override_enabled_ = false;
    counter_ = 0;
}

void LED_Controller::getCurrentRGB(uint8_t* r, uint8_t* g, uint8_t* b) {
    // Check override state first.
    if (is_override_enabled_) {
        *r = override_color_.r; *g = override_color_.g; *b = override_color_.b;
        return;
    }

    // If not overriding, compute current RGB value from base color and current
    // display type.
    int intensity = 0;
    switch (curr_display_type_) {
    case SOLID:
        *r = curr_foreground_color_.r; *g = curr_foreground_color_.g; *b = curr_foreground_color_.b;
        break;

    case FADE:
        intensity = counter_ - (LED_PERIOD / 2); // PERIOD / 2 to -PERIOD / 2
        if (intensity < 0) {
            intensity *= -1; // triangular distribution from 0 to PERIOD / 2
        }
        intensity = (intensity * 2 * 255) / LED_PERIOD; // scale to 0-255
        *r = (curr_foreground_color_.r * intensity) / 255 + (curr_background_color_.r * (255-intensity)) / 255;
        *g = (curr_foreground_color_.g * intensity) / 255 + (curr_background_color_.g * (255-intensity)) / 255;
        *b = (curr_foreground_color_.b * intensity) / 255 + (curr_background_color_.b * (255-intensity)) / 255;
        break;

    case FADE_FAST:
        intensity = (((6 * counter_) % LED_PERIOD) - (LED_PERIOD / 2)); // PERIOD / 2 to -PERIOD / 2
        if (intensity < 0) {
            intensity *= -1; // triangular distribution from 0 to PERIOD / 2
        }
        intensity = (intensity * 2 * 255) / LED_PERIOD; // scale to 0-255
        *r = (curr_foreground_color_.r * intensity) / 255 + (curr_background_color_.r * (255-intensity)) / 255;
        *g = (curr_foreground_color_.g * intensity) / 255 + (curr_background_color_.g * (255-intensity)) / 255;
        *b = (curr_foreground_color_.b * intensity) / 255 + (curr_background_color_.b * (255-intensity)) / 255;
        break;

    case BLINK:
        // note that guard will go from 0, 1 every half period
        // alternating the parity every half period
        if (((2 * counter_) / LED_PERIOD) % 2 == 0) {
            *r = curr_foreground_color_.r; *g = curr_foreground_color_.g; *b = curr_foreground_color_.b; // foreground
        } else {
            *r = curr_background_color_.r; *g = curr_background_color_.g; *b = curr_background_color_.b; // background
        }
        break;

    case BLINK_FAST:
        // note that guard will go from 0, 1, 2, 3. . . every 16th of a period
        // alternating the parity every 16th of a period
        if (((16 * counter_) / LED_PERIOD) % 2 == 0) {
            *r = curr_foreground_color_.r; *g = curr_foreground_color_.g; *b = curr_foreground_color_.b; // foreground
        } else {
            *r = curr_background_color_.r; *g = curr_background_color_.g; *b = curr_background_color_.b; // background
        }
        break;

    case OFF:
        *r = 0; *g = 0; *b = 0; // off
        break;
    }
}

void LED_Controller::update() {
    uint8_t r, g, b;
    getCurrentRGB(&r, &g, &b);
    led_driver_.setColor(r, g, b);
    counter_ = (counter_ + 1) % LED_PERIOD;
}

LED_Controller& LED_Controller::setColor(LedColorMode mode, uint8_t r, uint8_t g, uint8_t b)
{
    switch(mode) {
    case FOREGROUND:
        curr_foreground_color_.r = r;
        curr_foreground_color_.g = g;
        curr_foreground_color_.b = b;
        curr_background_color_.r = 0;
        curr_background_color_.g = 0;
        curr_background_color_.b = 0;
        break;
    case BACKGROUND:
        curr_background_color_.r = r;
        curr_background_color_.g = g;
        curr_background_color_.b = b;
        break;
    }
    setDisplayType(FADE);
    return *this;
}

void LED_Controller::getColor(uint8_t* r, uint8_t* g, uint8_t* b) {
    getCurrentRGB(r, g, b);
}

void LED_Controller::enableOverride(uint8_t r, uint8_t g, uint8_t b) {
    override_color_.r = r;
    override_color_.g = g;
    override_color_.b = b;
    is_override_enabled_ = true;
}

void LED_Controller::disableOverride() {
    is_override_enabled_ = false;
}

bool LED_Controller::isOverrideEnabled() {
    return is_override_enabled_;
}

void LED_Controller::getOverrideColor(uint8_t* r, uint8_t* g, uint8_t* b) {
    *r = override_color_.r;
    *g = override_color_.g;
    *b = override_color_.b;
}

LED_Controller& LED_Controller::setDisplayType(LedDisplayType type) {
    curr_display_type_ = type;
    return *this;
}

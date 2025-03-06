/**
 * @file LED_Controller.h
 * @brief Higher level controller for the onboard LEDs.
 *
 * Created: 28 May 2014
 * 
 * @author Oscar Bezi (bezi@cmu.edu), Matthew Tesch (mtesch@cmu.edu)
 */

#pragma once
 
#include "hardware/LED_RGB.h"
 
namespace hebi::firmware::modules {
 
const int LED_PERIOD = 2000;
 
class LED_Controller {
public:
    // When you set the color of the LED, you can set the foreground or background
    // (off) color.
    enum LedColorMode {BACKGROUND, FOREGROUND};
private:
    enum LedDisplayType {SOLID, FADE, FADE_FAST, BLINK, BLINK_FAST, OFF};
    struct Color {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };
 
    int counter_;
    hardware::LED_RGB& led_driver_;
    // The foreground color for the LED
    Color curr_foreground_color_;
    // The background color for the LED (e.g., what color is shown in the "off"
    // state).
    Color curr_background_color_;
    // Allows temporary override of the underlying control.  Suspends blink/fade/
    // etc. and replaces with a static color.  On disable, returns to prior
    // behavior.
    bool is_override_enabled_;
    // Color used when in override mode.
    Color override_color_;
    // The color that the LED should actually show, after fading/override/etc.
    void getCurrentRGB(uint8_t* r, uint8_t* g, uint8_t* b);
    LedDisplayType curr_display_type_;
    
    /*
    * @brief sets display type
    * @return reference to this controller, allowing for a.red().blink();, for
    *         example.
    */
    LED_Controller& setDisplayType(LedDisplayType type);
 
public:
    LED_Controller(hardware::LED_RGB& led_driver);
    /*
    * @brief run once per millisecond, updates the counter and blinks as needed
    */
    void update();
 
    /**
    * @brief sets the background or foreground LED color to the three specified
    * parameters.
    * Note that when setting a color, the "fade" display type is resumed.
    * Note that when setting the foreground color, the background color is
    * cleared (this ensures support for legacy calls, so that a.red().fade()
    * always alternates between red and off).
    * @return reference to this controller, allowing for a.red().blink();, for
    *         example.
    */
    LED_Controller& setColor(LedColorMode mode, uint8_t r, uint8_t g, uint8_t b);
 
    /*
    * @brief gets the actual color that the LED is displaying
    */
    void getColor(uint8_t* r, uint8_t* g, uint8_t* b);
    
    /*
    * @brief Allows temporary override of the underlying control.  Suspends
    * blink/fade/etc. and replaces with a static color. If already in override
    * mode, sets new color.  Keeps base state unchanged.
    */
    void enableOverride(uint8_t r, uint8_t g, uint8_t b);
    /*
    * @brief If in override mode, resets to prior behavior.  Otherwise, does
    * nothing.
    */
    void disableOverride();
    bool isOverrideEnabled();
    /*
    * @brief gets the override color for the LED.  If override is disabled, the
    * outputs are undefined.
    */
    void getOverrideColor(uint8_t* r, uint8_t* g, uint8_t* b);
 
    /*
    * @brief the following functions set the display type to the respective enum
    *        values.  Wrappers around setDisplayType();
    * @return reference to this controller, allowing for a.red().blink();, for
    *         example.
    */
    LED_Controller& solid() { return setDisplayType(SOLID); }
    LED_Controller& fade() { return setDisplayType(FADE); }
    LED_Controller& fadeFast() { return setDisplayType(FADE_FAST); }
    LED_Controller& blink() { return setDisplayType(BLINK); }
    LED_Controller& blinkFast() { return setDisplayType(BLINK_FAST); }
    LED_Controller& off() { return setDisplayType(OFF); }
 
    /*
    * @brief the following functions set the color.  Wrappers around setColor();
    * @return reference to this controller, allowing for a.red().blink();, for
    *         example.
    */
    LED_Controller& white(LedColorMode mode = FOREGROUND) { return setColor(mode, 255, 255, 255); } 
    LED_Controller& red(LedColorMode mode = FOREGROUND) { return setColor(mode, 255, 0, 0); } 
    LED_Controller& green(LedColorMode mode = FOREGROUND) { return setColor(mode, 0, 255, 0); } 
    LED_Controller& blue(LedColorMode mode = FOREGROUND) { return setColor(mode, 0, 0, 255); } 
    LED_Controller& yellow(LedColorMode mode = FOREGROUND) { return setColor(mode, 255, 255, 0); } 
    LED_Controller& purple(LedColorMode mode = FOREGROUND) { return setColor(mode, 255, 0, 255); } 
    LED_Controller& teal(LedColorMode mode = FOREGROUND) { return setColor(mode, 0, 255, 255); } 
    LED_Controller& orange(LedColorMode mode = FOREGROUND) { return setColor(mode, 255, 77, 0); }
};

} //namespace hebi::firmware::modules
 
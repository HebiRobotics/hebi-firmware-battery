/**
 * LED_RGB.h
 * 
 * Simple interface class for an RGB LED
 */

#pragma once

#include <stdint.h>

namespace hebi::firmware::hardware {

class LED_RGB {
public:
    inline uint8_t r() const { return r_; }
    inline uint8_t g() const { return g_; }
    inline uint8_t b() const { return b_; }

    virtual void getColor(uint8_t& r, uint8_t& b, uint8_t &g) {
        r = r_;
        g = g_;
        b = b_;
    }

    virtual void setColor(uint8_t r, uint8_t g, uint8_t b) {
        r_ = r;
        g_ = g;
        b_ = b;
        colorUpdated();
    }

protected:
    virtual void colorUpdated() = 0;

    uint8_t r_ {0};
    uint8_t g_ {0};
    uint8_t b_ {0};
};

} //namespace hebi::firmware::hardware

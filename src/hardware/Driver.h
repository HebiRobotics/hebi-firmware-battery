/**
 * Driver.h
 * 
 * Generic interface class to allow driver enabling and disabling
 */

#pragma once

namespace hebi::firmware::hardware {

class Driver {
public:
    virtual void startDriver() = 0;
    virtual void stopDriver() = 0;
};
 
 } //namespace hebi::firmware::hardware
 
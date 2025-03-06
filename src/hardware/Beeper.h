/**
 * Beeper.h
 * 
 * Simple interface class for a single-frequency speaker
 */

#pragma once

#include <stdint.h>

namespace hebi::firmware::hardware {

class Beeper {
public:
    virtual void startBeep() = 0;
    virtual void stopBeep() = 0;
};
 
 } //namespace hebi::firmware::hardware
 
#pragma once
#include <Arduino.h>

struct Ballast {
    byte high;
    byte middle;
    byte low;
    uint8_t address = 255;
};
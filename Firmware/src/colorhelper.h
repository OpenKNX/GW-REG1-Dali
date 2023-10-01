#pragma once

#include "Arduino.h"

class ColorHelper
{
    public:
        static void rgbToXY(uint8_t r, uint8_t g, uint8_t b, uint16_t& x, uint16_t& y);
        static void hslToRGB(uint8_t h, uint8_t s, uint8_t l, uint8_t& r, uint8_t& g, uint8_t& b);

    private:
        static uint16_t getBytes(float input);
        static double hue2rgb(double p, double q, double t);
};
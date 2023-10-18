#pragma once

#include "Arduino.h"

//extended lib from https://github.com/ratkins/RGBConverter
//WTFPL license

class ColorHelper
{
    public:
        static void rgbToXY(uint8_t r, uint8_t g, uint8_t b, uint16_t& x, uint16_t& y);
        static void hsvToRGB(uint8_t h, uint8_t s, uint8_t v, uint8_t& r, uint8_t& g, uint8_t& b);
        static void kelvinToRGB(uint16_t kelvin, uint8_t& r, uint8_t& g, uint8_t& b);
    private:
        static uint16_t getBytes(float input);
        static double hue2rgb(double p, double q, double t);
};
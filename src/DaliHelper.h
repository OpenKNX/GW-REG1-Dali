#pragma once

#include "Arduino.h"

class DaliHelper
{
    public:
		static uint8_t percentToArc(uint8_t value);
		static uint8_t arcToPercent(uint8_t value);
		static uint8_t roundToInt(double input);
};
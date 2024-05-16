#pragma once

#include <Arduino.h>
#include <string>
#include "OpenKNX.h"
#include "knxprod.h"
#include "TimerModule.h"
#include "colorhelper.h"

class HclCurve
{
    public:
        void setup(uint8_t index);
        void loop();

    private:
        const std::string logPrefix();
        const uint8_t channelIndex();
        uint8_t _index = 0;
        bool _isConfigured = false;
        uint8_t _type = 0;

        unsigned long _lastCheck = 5000000;
        uint8_t _lastDay = 0;
        uint8_t _lastMinute = 0;
};
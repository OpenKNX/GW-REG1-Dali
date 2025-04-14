#pragma once

#ifdef ARDUINO_ARCH_RP2040
    #include "OpenKnxHardware.h"
    #define REG1_APP_PIN6 17
    #define REG1_APP_PIN7 16
#elif defined(ARDUINO_ARCH_ESP32)
    #include "HardwareConfig.h"
#endif
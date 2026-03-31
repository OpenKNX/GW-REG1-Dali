#include "FileTransferModule.h"
#include "OpenKNX.h"
#include "DaliModule.h"
#include "Logic.h"

#ifdef ARDUINO_ARCH_ESP32
#include "NetworkModule.h"
#endif

void setup()
{
    openknx.init();
    openknx.addModule(1, openknxLogic);
    openknx.addModule(3, openknxDaliModule);
    openknx.addModule(9, openknxFileTransferModule);
    #ifdef ARDUINO_ARCH_ESP32
        openknx.addModule(2, openknxNetwork);
    #endif
    openknx.setup();
    
    #ifdef ARDUINO_ARCH_ESP32
    xTaskCreateUniversal([](void* parms) {
        openknxDaliModule.setup1(true);
        for (;;)
        {
            openknxDaliModule.loop1(true);
        } 
    }, "daliModuleLoop1", 8192, NULL, 0, nullptr, 0);
    #endif
}

void loop()
{
    openknx.loop();
}

#if defined(ARDUINO_ARCH_RP2040)
    void setup1() {
        openknxDaliModule.setup1(true);
    }

    void loop1() {
        openknxDaliModule.loop1(true);
    }
#endif
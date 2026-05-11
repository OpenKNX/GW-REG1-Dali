#include "FileTransferModule.h"
#include "OpenKNX.h"
#include "DaliModule.h"
#include "Logic.h"

#ifdef ARDUINO_ARCH_ESP32
#include "NetworkModule.h"
#endif

volatile bool setup_ready = false;

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
    openknxDaliModule.setup1(knx.configured());

    #ifdef ARDUINO_ARCH_ESP32
    xTaskCreateUniversal([](void* parms) {
        for (;;)
        {
            openknxDaliModule.loop1(knx.configured());
        } 
    }, "daliModuleLoop1", 8192, NULL, 0, nullptr, 0);
    #endif

    setup_ready = true;
}

void loop()
{
    openknx.loop();
}

#if defined(ARDUINO_ARCH_RP2040)
    void setup1() { }

    void loop1() {
        while(!setup_ready) {
            delay(10);
        }
        openknxDaliModule.loop1(knx.configured());
    }
#endif
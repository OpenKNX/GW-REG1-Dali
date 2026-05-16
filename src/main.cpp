#include "FileTransferModule.h"
#include "OpenKNX.h"
#include "DaliModule.h"
#include "Logic.h"

#ifdef ARDUINO_ARCH_ESP32
#include "NetworkModule.h"
#include "IotGateway.h"
IotGateway iotGateway;
#endif

bool setup0_ready = false;
bool setup1_ready = false;

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
        iotGateway.setup();
        iotGateway.addMaster(&openknxDaliModule.daliMaster);
    #endif

    setup0_ready = true;

    #ifdef ARDUINO_ARCH_ESP32
    // Pin DALI loop to APP_CPU (core 1) with elevated priority so it is not
    // preempted by WiFi/IP/Ethernet activity on PRO_CPU (core 0) nor by the
    // Arduino loopTask. Without this, the RMT RX re-arm (Rmt.cpp) misses the
    // backward-frame window during ETS-triggered scans → truncated frames
    // (bits=6/7/0). Equivalent to the RP2040 setup1()/loop1() multicore fix
    // (OFM-Dali commit fb3aea9).
    xTaskCreateUniversal([](void* parms) {
        while(!setup0_ready) {
            delay(10);
        }
        openknxDaliModule.setup1(knx.configured());
        setup1_ready = true;
        for (;;)
        {
            openknxDaliModule.loop1(knx.configured());
            vTaskDelay(1);  // yield to lower-prio tasks on this core
        }
    }, "daliModuleLoop1", 8192, NULL, 5, nullptr, 1);
    //                          ^prio=5  ^core=APP_CPU (opposite of WiFi/IP)
    #endif

    while(!setup1_ready) {
        delay(10);
    }
}

void loop()
{
    openknx.loop();
}

#if defined(ARDUINO_ARCH_RP2040)
    void setup1() {
        while(!setup0_ready) {
            delay(10);
        }
        openknxDaliModule.setup1(knx.configured());
        setup1_ready = true;
    }

    void loop1() {
        openknxDaliModule.loop1(knx.configured());
    }
#endif
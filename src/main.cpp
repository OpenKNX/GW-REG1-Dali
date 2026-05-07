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
}

void loop()
{
    openknx.loop();
}
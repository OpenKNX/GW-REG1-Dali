#include "FileTransferModule.h"
#include "InfraredModule.h"
#include "OpenKNX.h"
#include "VirtualButtonModule.h"
#include "Logic.h"

void setup()
{
    const uint8_t firmwareRevision = 2;
    openknx.init(firmwareRevision);
    openknx.addModule(1, openknxLogic);
    openknx.addModule(3, openknxVirtualButtonModule);
    openknx.addModule(4, openknxInfraredModule);
    openknx.addModule(9, openknxFileTransferModule);
    openknx.setup();
}

void loop()
{
    openknx.loop();
}
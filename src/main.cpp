#include <Arduino.h>
#include "TimerInterrupt_Generic.h"
#include "OpenKNX.h"
#include "DaliModule.h"
#ifdef ARDUINO_ARCH_RP2040
#include "FileTransferModule.h"
#endif


void daliCallback(uint8_t *data, uint8_t len)
{
	logHexInfo("Dali In", data, len);
}

void setup()
{
	const uint8_t firmwareRevision = 0;
	openknx.init(firmwareRevision);
	//openknxDaliModule.setCallback(daliCallback);
	openknx.addModule(1, openknxDaliModule);
#ifdef ARDUINO_ARCH_RP2040
	openknx.addModule(3, openknxFileTransferModule);
#endif
	openknx.setup();
}

void setup1()
{
	openknx.setup1();
}

void loop()
{
	openknx.loop();
}

void loop1()
{
	openknx.loop1();
}
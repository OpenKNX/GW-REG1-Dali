#include <Arduino.h>
#include "TimerInterrupt_Generic.h"
#include "OpenKNX.h"
#include "DaliModule.h"
#include "TimerModule.h"
#ifdef ARDUINO_ARCH_RP2040
#include "FileTransferModule.h"
#endif


void daliCallback(uint8_t *data, uint8_t len)
{
	logHexInfo("Dali In", data, len);
}

bool setup0ready = false;
bool setup1ready = false;

void setup()
{
	const uint8_t firmwareRevision = 2;
	openknx.init(firmwareRevision);
	//openknxDaliModule.setCallback(daliCallback);
	openknx.addModule(1, openknxDaliModule);
	openknx.addModule(2, openknxTimerModule);
#ifdef ARDUINO_ARCH_RP2040
	openknx.addModule(3, openknxFileTransferModule);
#endif
	openknx.setup();

	setup0ready = true;

	while(!setup1ready)
		delay(1);
}

void loop()
{
	openknx.loop();
}

void setup1()
{
	while(!setup0ready)
		delay(1);

	openknxDaliModule.setup1(knx.configured());

	setup1ready = true;
}

void loop1()
{
	// openknx.loop1();
	openknxDaliModule.loop1(knx.configured());
}
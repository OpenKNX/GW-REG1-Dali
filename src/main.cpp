#include <Arduino.h>
#include "TimerInterrupt_Generic.h"
#include "OpenKNX.h"
#include "Dali/DaliModule.h"
#ifdef ARDUINO_ARCH_RP2040
#include "FileTransferModule.h"
#endif

#ifdef USE_LUBA_PROTOCOLL
#include "Luba/LubaProtocoll.hpp"
#endif

void daliCallback(uint8_t *data, uint8_t len)
{
	logHexInfo("Dali In", data, len);
}

bool setup0ready = false;
bool setup1ready = false;

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

	setup0ready = true;

	Serial.end();
	Serial.begin(38400, SERIAL_8N1);

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
	//openknx.loop1();
	openknxDaliModule.loop1(knx.configured());

	#ifdef USE_LUBA_PROTOCOLL
	loopLubaProtocoll();
	#endif
}
#include <Arduino.h>
#include "TimerInterrupt_Generic.h"
#include "OpenKNX.h"
#include "DaliModule.h"
#include "FileTransferModule.h"


void daliCallback(uint8_t *data, uint8_t len)
{
	logHexInfo("Dali In", data, len);
}

void setup()
{
	//Serial2.setTX(8);
	//Serial2.setRX(9);

	const uint8_t firmwareRevision = 0;
	openknx.init(firmwareRevision);
	openknxDaliModule.setCallback(daliCallback);
	openknx.addModule(1, &openknxDaliModule);
	openknx.addModule(3, &openknxFileTransferModule);
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
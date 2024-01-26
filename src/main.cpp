#include <Arduino.h>
#include "TimerInterrupt_Generic.h"
#include "OpenKNX.h"
#include "DaliModule.h"
#include "FileTransferModule.h"

unsigned long daliActivity = 0;

void daliCallback(uint8_t *data, uint8_t len)
{
	logHexInfo("Dali In", data, len);
}

void setup()
{
	const uint8_t firmwareRevision = 1;
	openknx.init(firmwareRevision);
	//openknxDaliModule.setCallback(daliCallback);
	openknxDaliModule.setActivityCallback([]() {
		daliActivity = millis();
	});
	openknx.addModule(1, openknxDaliModule);
	openknx.addModule(3, openknxFileTransferModule);
	openknx.setup();

#ifdef OKNXHW_REG1_BASE_V1
	openknx.info3Led.activity(daliActivity, false);
#else
	openknx.info1Led.activity(daliActivity, false);
#endif
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
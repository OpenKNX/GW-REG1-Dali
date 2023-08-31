#include <Arduino.h>
#include "TimerInterrupt_Generic.h"
#include "OpenKNX.h"
#include "DaliModule.h"
#include "FileTransferModule.h"


#ifdef USE_TINYUSB
#include "HidController.hpp"
HidController *hid;

void setup1()
{
	Serial.end();
	hid = new HidController();
	openknx.setup1();
}
#else
void setup1()
{
	openknx.setup1();
}
#endif

void daliCallback(uint8_t *data, uint8_t len)
{
#ifdef USE_TINYUSB
	hid->daliCallback(data, len);
#endif
	logHexInfo("Dali In", data, len);
}

void setup()
{
	#ifdef USE_TINYUSB
	Serial2.setTX(8);
	Serial2.setRX(9);
	#endif

	const uint8_t firmwareRevision = 0;
	openknx.init(firmwareRevision);
	DaliModule *mod = new DaliModule();
	mod->setCallback(daliCallback);
	openknx.addModule(1, mod);
	openknx.addModule(3, new FileTransferModule());
	openknx.setup();
}

void loop()
{
	openknx.loop();
}

void loop1()
{
	openknx.loop1();
}
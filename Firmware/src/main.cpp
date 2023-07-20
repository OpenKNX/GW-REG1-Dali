#include <Arduino.h>
#include "TimerInterrupt_Generic.h"
#include "OpenKNX.h"
#include "pins.h"
#include "dali.h"
#include "DaliModule.h"
#include "UpdaterModule.h"
#include "FTPServer.h"


#ifdef USE_TINYUSB
#include "HidController.hpp"
HidController hid;

void setup1()
{
	Serial.end();
	openknx.setup1();
}

void daliCallback(uint8_t *data, uint8_t len)
{
	hid.daliCallback(data, len);
}
#endif

void setup()
{
	#ifdef USE_TINYUSB
	Serial2.setTX(16);//28);
	Serial2.setRX(17);//29);
	#endif

	const uint8_t firmwareRevision = 0;
	openknx.init(firmwareRevision);
#ifdef USE_TINYUSB
	DaliModule *mod = new DaliModule();
	mod->setCallback(daliCallback);
	openknx.addModule(1, mod);
#else
	openknx.addModule(1, new DaliModule());
#endif
	openknx.addModule(2, new UpdaterModule());
	openknx.addModule(3, new FtpServer());
	openknx.setup();
}

unsigned long lastmillis = 0;
bool state = true;
int addr = 0;

void loop()
{
	openknx.loop();
}

void loop1()
{
	openknx.loop1();
}
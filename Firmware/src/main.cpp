#include <Arduino.h>
#include "TimerInterrupt_Generic.h"
#include "OpenKNX.h"
#include "DaliModule.h"
#include "UpdaterModule.h"
#include "FTPServer.h"


#ifdef USE_TINYUSB
#include "HidController.hpp"
HidController *hid;

void setup1()
{
	Serial.end();
	hid = new HidController();
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
	Serial2.setTX(8);//28);
	Serial2.setRX(9);//29);
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
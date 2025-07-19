#include <Arduino.h>
#include "OpenKNX.h"
#include "DaliModule.h"
#include "TimerModule.h"
#include "FileTransferModule.h"
#ifdef ARDUINO_ARCH_ESP32
#include "NetworkModule.h"
#include "IotGateway.h"
IotGateway iotGateway;
#endif

void setup()
{
	const uint8_t firmwareRevision = 3;
	openknx.init(firmwareRevision);
	openknx.addModule(1, openknxDaliModule);
	openknx.addModule(2, openknxTimerModule);
	openknx.addModule(3, openknxFileTransferModule);
	#ifdef ARDUINO_ARCH_ESP32
	openknx.addModule(4, openknxNetwork);
	#endif

	openknx.setup();

	#ifdef ARDUINO_ARCH_ESP32
	iotGateway.setup();
	iotGateway.addMaster(&openknxDaliModule.daliMaster);
	#endif
}

void loop()
{
	openknx.loop();
}
#include <Arduino.h>
#include "TimerInterrupt_Generic.h"
#include "OpenKNX.h"
#include "pins.h"
#include "dali.h"
#include "DaliModule.h"
#include "UpdaterModule.h"
#include <hid/Adafruit_USBD_HID.h>


uint8_t const desc_hid_report[] =
{
	TUD_HID_REPORT_DESC_GENERIC_INOUT(64)
};

// USB HID object. For ESP32 these values cannot be changed after this declaration
// desc report, desc len, protocol, interval, use out endpoint
Adafruit_USBD_HID usb_hid(desc_hid_report, sizeof(desc_hid_report), HID_ITF_PROTOCOL_NONE, 2, true);

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t get_report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen)
{
	// not used in this example
	(void)report_id;
	(void)report_type;
	(void)buffer;
	(void)reqlen;
	logInfo("HID", "get report callback");
	return 0;
}

void handleConfig(uint8_t const *buffer)
{
	int x = buffer[1] << 8;
	x |= buffer[2];

	switch (x)
	{
	// version info
	case 0x0000:
	{
		uint8_t *report = new uint8_t[63];
		report[0] = 0x00;
		report[1] = 0x20;
		report[2] = 0x02;
		report[3] = 0x02;
		report[4] = 0x10;
		for (int i = 5; i < 63; i++)
			report[i] = 0x00;
		usb_hid.sendReport(1, report, 63);
		delete[] report;
		break;
	}

	// dont know
	// internal flash not showing
	case 0x0100:
	{
		uint8_t *report = new uint8_t[63];
		report[0] = 0x00;
		report[1] = 0x00;
		report[2] = 0x02;
		report[3] = 0x20;
		for (int i = 4; i < 63; i++)
			report[i] = 0x00;
		usb_hid.sendReport(1, report, 63);
		delete[] report;
		break;
	}
	}
}



byte *message = new byte[2];
uint8_t sequence = 0;
bool sendMessage = false;

void handleSend(uint8_t const *buffer)
{
	logInfo("HID", "its really for me");

	logHexInfo("DALI", buffer, 20);
	logHexInfo("DALI", buffer + 6, 2);

	if (buffer[6] >> 7 == 0)
	{
		int x = buffer[6] >> 1 & 0b111111;
		logInfo("HID", "Got ShortAddress A%i", x);
	} else if(buffer[6] >> 5 == 0b100) {
		int x = buffer[6] >> 1 & 0b1111;
		logInfo("HID", "Got GroupAddress G%i", x);
	} else if(buffer[6] >> 5 == 0b111) {
		logInfo("HID", "Got Broadcast");
	}

	message[0] = buffer[6];
	message[1] = buffer[7];
	sequence = buffer[1];
	logHexInfo("DALI", message, 2);
	//DaliBus.sendRaw(message, 2);
	sendMessage = true;
	return;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void set_report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
{
	switch (buffer[0])
	{
		// interface config
		case 0x01:
		{
			handleConfig(buffer);
			break;
		}

		// dali bus related
		case 0x12:
		{
			int x = buffer[2] << 8;
			x |= buffer[3];
			if (x == 0x0003) // send to dali
			{
				handleSend(buffer);
			}
			else if (x == 0x4000 && buffer[1] == 0x5a)
			{ // dont know / bus info
				uint8_t *report = new uint8_t[63];
				report[0] = 0x77;
				report[1] = 0x00;
				report[2] = 0x00;
				report[3] = 0x00;
				report[4] = 0x04; //04: Busspannung da | 02: keine Busspannung
				report[5] = 0xff; //ff
				report[6] = 0xff; //ff
				report[7] = buffer[1]; //sequence
				for (int i = 8; i < 63; i++)
					report[i] = 0x00;
				usb_hid.sendReport(0x12, report, 63);
				delete[] report;
				break;
			}
			break;
		}

		// dont know
		case 0x50:
		{
			uint8_t *report = new uint8_t[63];
			report[0] = 0x00;
			report[1] = 0x00;
			report[2] = 0x00;
			report[3] = 0x01;
			for (int i = 4; i < 63; i++)
				report[i] = 0x00;
			usb_hid.sendReport(0xaf, report, 63);
			delete[] report;
			break;
		}
	}
}


bool DaliBus_wrapper_timerISR(struct repeating_timer *t) { DaliBus.timerISR(); return true;}

void setup1()
{
	Serial.end();
	//set vid and pid for lunatone usb maus
	TinyUSBDevice.setID(0x17B5, 0x0020); // 0x1234, 0xabcd);
	TinyUSBDevice.setProductDescriptor("Pico Dual Gamepad"); //doesnt work
	usb_hid.setStringDescriptor("OpenKNX DALI USB"); //doesnt work
	usb_hid.setReportCallback(get_report_callback, set_report_callback);
	usb_hid.begin();
}

void setup()
{
	Serial2.setTX(4);//20); //4);
	Serial2.setRX(5);//21); //5);
	SERIAL_DEBUG.begin(115200);

	const uint8_t firmwareRevision = 0;
	openknx.init(firmwareRevision);
	openknx.addModule(1, new DaliModule());
	openknx.addModule(2, new UpdaterModule());
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
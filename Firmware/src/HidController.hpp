#include "Arduino.h"
#include <hid/Adafruit_USBD_HID.h>


class HidController
{
    public:
        HidController();
        inline static HidController *Instance = nullptr;
        //uint16_t get_report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen);
        void report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize);
        void daliCallback(uint8_t *data, uint8_t len);

    private:
        void handleSend(uint8_t const *buffer);
        void handleConfig(uint8_t const *buffer);
        Adafruit_USBD_HID *usb_hid;
};

uint16_t get_report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen)
{
    return 0; //HidController::Instance->get_report_callback(report_id, report_type, buffer, reqlen);
}

void set_report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
{
    return HidController::Instance->report_callback(report_id, report_type, buffer, bufsize);
}

HidController::HidController()
{
    HidController::Instance = this;

    uint8_t const desc_hid_report[] = {TUD_HID_REPORT_DESC_GENERIC_INOUT(64)};
    usb_hid = new Adafruit_USBD_HID(desc_hid_report, sizeof(desc_hid_report), HID_ITF_PROTOCOL_NONE, 2, true);

    TinyUSBDevice.setID(0x17B5, 0x0020); // 0x1234, 0xabcd);
	//TinyUSBDevice.setProductDescriptor("Pico Dual Gamepad"); //doesnt work
	//usb_hid->setStringDescriptor("OpenKNX DALI USB"); //doesnt work
	usb_hid->setReportCallback(get_report_callback, set_report_callback);
	usb_hid->begin();
}

void HidController::daliCallback(uint8_t *data, uint8_t len)
{
    uint8_t *report = new uint8_t[63];
    report[0] = 0x00;
    report[1] = 0x00;
    report[2] = 0x00;
    report[3] = 0x01;
    //todo add all things here
    for (int i = 4; i < 63; i++)
        report[i] = 0x00;
    usb_hid->sendReport(0xaf, report, 63);
    delete[] report;
}

void HidController::handleConfig(uint8_t const *buffer)
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
            usb_hid->sendReport(1, report, 63);
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
            usb_hid->sendReport(1, report, 63);
            delete[] report;
            break;
        }
	}
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void HidController::report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
{
	logHexInfo("HID", buffer, 20);

	switch (buffer[0])
	{
		// interface config
		case 0x01:
		{
			logInfo("HID", "config");
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
				logInfo("HID", "send to dali");
				handleSend(buffer);
			}
			else if (x == 0x4000 && buffer[1] == 0x5a)
			{ // dont know / bus info
				logInfo("HID", "bus info");
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
				usb_hid->sendReport(0x12, report, 63);
				delete[] report;
				break;
			} else {
				logInfo("HID", "unknown");
			}
			break;
		}

		// dont know
		case 0x50:
		{
			logInfo("HID", "dont know");
			uint8_t *report = new uint8_t[63];
			report[0] = 0x00;
			report[1] = 0x00;
			report[2] = 0x00;
			report[3] = 0x01;
			for (int i = 4; i < 63; i++)
				report[i] = 0x00;
			usb_hid->sendReport(0xaf, report, 63);
			delete[] report;
			break;
		}
	}
}

void HidController::handleSend(uint8_t const *buffer)
{
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

    byte *message = new byte[2];
	message[0] = buffer[6];
	message[1] = buffer[7];
	logHexInfo("DALI", message, 2);
	int result = DaliBus.sendRaw(message, 2);
    delete[] message;

    unsigned long time = millis();

    while (!DaliBus.busIsIdle())
    if (millis() - time > 50) return;

    if(result == DALI_SENT && DaliBus.getLastResponse() > 0)
    {
        byte response = DaliBus.getLastResponse();
        daliCallback(&response, 1);
    }
}

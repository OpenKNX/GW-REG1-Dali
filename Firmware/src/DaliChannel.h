#pragma once

#include "OpenKNX.h"
#include "Dali.h"
#include "MessageQueue.h"
#include "colorhelper.h"
#include "DaliCommands.h"
#include "DaliHelper.h"

#define DimmInterval 100

class DaliChannel : public OpenKNX::Channel
{
	public:
        DaliChannel(MessageQueue &queue);
        ~DaliChannel();

		void loop() override;
		void loop1() override;
		void setup() override;
		void processInputKo(GroupObject &ko) override;
		const bool isConfigured();
		const bool isGroup();

		void init(uint8_t channelIndex, bool isGroup);
		void setOnValue(uint8_t value);
		void setGroups(uint16_t groups);
		void setGroupState(uint8_t group, bool state);
		void setGroupState(uint8_t group, uint8_t value);
		void setMinMax(uint8_t min, uint8_t max);
		uint8_t getMin();
		uint8_t getMax();
		uint16_t getGroups();

		bool isNight = false;
		const std::string name() override;
		// void writeFlash() override;
		// void readFlash(const uint8_t* data, const uint16_t size) override;
		// uint16_t flashSize() override;


	private:
		enum class DimmDirection {
			Down,
			Up,
			None
		};
		enum class DimmType {
			Brigthness,
			Color
		};

		MessageQueue &_queue;

		//relatives Dimmen
		DimmDirection _dimmDirection = DimmDirection::None;
		uint8_t _dimmStep = 0;
		unsigned long _dimmLast = 0;
		//Treppenlicht
		unsigned long startTime = 0;
		uint interval = 0;
		//Initialwerte
		uint8_t _min = 0;
		uint8_t _max = 0;
		uint8_t _onDay = 100;
		uint8_t _onNight = 10;
		bool _isStaircase = false;
		bool _isGroup = false;
		bool _isConfigured = false;
		//EVG Fehler auslesen
		bool _getError = false;
		uint16_t _errorResp = 300;
		unsigned long _lastError = 0;
		//Aktueller Status
		bool currentState = false;
		//Aktuelle Helligkeit
		uint8_t currentStep = 0;
		//Aktueller Sperrstatus
		bool currentIsLocked = false;
		//Aktuelle Farbe
		uint8_t currentColor[3];
		//Aktuell zu dimmender Wert
		uint8_t *currentDimmValue;
		DimmType currentDimmType;
		unsigned long lastCurrentDimmUpdate = 0;

		//Einschalten mit letztem Wert
		uint8_t _lastDayValue = 100;
		uint8_t _lastNightValue = 10;

		//Gruppenzugehörigkeit
		uint16_t _groups = 0;


		uint16_t calcKoNumber(int asap);
		uint8_t sendArc(byte value);
		uint8_t sendCmd(byte cmd);
		uint8_t sendSpecialCmd(DaliSpecialCmd cmd, byte value);
		void setSwitchState(bool value, bool isSwitchCommand = true);
		void setDimmState(uint8_t value, bool isDimmCommand = true, bool isLastCommand = false);
		void updateCurrentDimmValue(bool isLastCommand);
		void sendColor(bool isLastCommand);
		
		void koHandleSwitch(GroupObject &ko);
		void koHandleDimmRel(GroupObject &ko);
		void koHandleDimmAbs(GroupObject &ko);
		void koHandleLock(GroupObject & ko);
		void koHandleColor(GroupObject &ko);
		void koHandleColorRel(GroupObject &ko, uint8_t index);
		void koHandleColorAbs(GroupObject &ko, uint8_t index);
};
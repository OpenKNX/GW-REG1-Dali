#pragma once

#include "OpenKNX.h"
#include "Dali.h"
#include "MessageQueue.h"
#include "colorhelper.h"
#include "DaliCommands.h"

#define DimmInterval 100

class DaliChannel : public OpenKNX::Channel
{
	public:
        DaliChannel(uint8_t channelIndex, MessageQueue *queue, bool isGroup);
        ~DaliChannel();

		void loop() override;
		void loop1() override;
		void setup() override;
		void processInputKo(GroupObject &ko) override;
		const bool isConfigured();
		const bool isGroup();

		void setOnValue(uint8_t value);
		void setGroups(uint16_t groups);
		void setGroupState(uint8_t group, bool state);
		void setGroupState(uint8_t group, uint8_t value);

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

		MessageQueue *_queue;
		DimmDirection _dimmDirection = DimmDirection::None;
		uint8_t _dimmStep = 0;
		unsigned long _dimmLast = 0;
		bool _isConfigured = false;
		bool state = false;
		bool _isGroup = false;
		bool isLocked = false;
		unsigned long startTime = 0;
		uint interval = 0;
		uint8_t _min = 0;
		uint8_t _max = 0;
		uint8_t _onDay = 100;
		uint8_t _onNight = 10;
		bool canReTrigger = false;
		bool _isStaircase = false;
		bool _getError = false;
		uint16_t _errorResp = 300;
		unsigned long _lastError = 0;
		uint8_t _lastValue = 0;
		bool _lastState = false;
		uint8_t _lastStep = 0;
		uint8_t _lastDayValue = 100;
		uint8_t _lastNightValue = 10;

		uint16_t _groups = 0;

		uint16_t calcKoNumber(int asap);
		uint8_t sendArc(byte value);
		uint8_t sendCmd(byte cmd);
		uint8_t sendSpecialCmd(DaliSpecialCmd cmd, byte value);
		uint8_t percentToArc(uint8_t value);
		uint8_t arcToPercent(uint8_t value);
		uint8_t roundToInt(double input);
		void setSwitchState(bool value, bool isSwitchCommand = true);
		void setDimmState(uint8_t value, bool isDimmCommand = true, bool isLastCommand = false);
		
		void koHandleSwitch(GroupObject &ko);
		void koHandleDimmRel(GroupObject &ko);
		void koHandleDimmAbs(GroupObject &ko);
		void koHandleLock(GroupObject & ko);
		void koHandleColor(GroupObject &ko);
};
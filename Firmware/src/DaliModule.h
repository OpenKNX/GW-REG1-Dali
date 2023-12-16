#pragma once

#include <Arduino.h>
#include "OpenKNX.h"
#include "Dali.h"
#include "MessageQueue.h"
#include "DaliChannel.h"
#include "Ballast.hpp"
#include "DaliCommands.h"

#define DALI_TX 17
#define DALI_RX 16

#ifndef DALI_WAIT_RANDOMIZE
#define DALI_WAIT_RANDOMIZE 1000
#endif
#ifndef DALI_WAIT_SEARCH
#define DALI_WAIT_SEARCH 300
#endif

typedef void (*EventHandlerChangedGroupFuncPtr)(uint8_t index, uint8_t value);

class DaliModule : public OpenKNX::Module
{
	public:

		void loop() override;
		void loop1(bool configured) override;
		void setup(bool conf) override;
		void setup1(bool conf) override;
		bool processCommand(const std::string cmd, bool diagnoseKo) override;
		void processInputKo(GroupObject &ko) override;
		void setCallback(EventHandlerReceivedDataFuncPtr callback);

		const std::string name() override;
		const std::string version() override;

		bool getDaliBusState();

		bool processFunctionProperty(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t *data, uint8_t *resultData, uint8_t &resultLength) override;
		bool processFunctionPropertyState(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t *data, uint8_t *resultData, uint8_t &resultLength) override;

	private:
		enum class AddressingState {
			None,
			//addressing
			Init,
			Randomize_Wait,
			Search,
			SearchWait,
			Found,
			Finish,
			//addressing auto
			SearchAdr,
			SearchAdrWait,
			GetHWait,
			GetLWait,
			GetMWait,
			//assigning
			Query_Wait,
			Withdraw_Others,
			Set_Address,
			Check_Address,
			Confirm_Address
		};
		enum class AssigningState {
			None,
			Working,
			Success,
			Failed_Bus = 10,
			Failed_Exists,
			Failed_Exists_Not,
			Failed_Confirm,
			Failed_No_Answer
		};
		
		void loopAddressing();
		void loopMessages();
		void loopBusState();
		void loopInitData();
		int16_t getInfo(byte address, int command, uint8_t additional = 0);
	
		uint32_t _adrLow = 0;
		uint32_t _adrHigh = 0xFFFFFF;
		uint32_t _adrHighLast = 0xFFFFFF;
		int _adrResp = -1;
		int _adrNoRespCounter = 0;
		unsigned long _adrTime = 0;
		AddressingState _adrState = AddressingState::None;
		AssigningState _assState = AssigningState::None;
		Ballast *ballasts;
		bool *addresses;
		int _adrFound = 0;
		bool _adrAssign = false;

		uint8_t _lastChangedGroup = 255;
		uint8_t _lastChangedValue = 0;

		bool _gotInitData = false;
		bool _daliBusState = true;
		bool _daliBusStateToSet = true;
		unsigned long _daliStateLast = 1;
		DaliClass *dali;
		DaliChannel channels[64];
		DaliChannel groups[16];
		MessageQueue *queue;
		struct repeating_timer _timer;

		uint8_t sendMsg(MessageType t, byte addr, byte v, byte type = 0, bool wait = false);
		uint8_t sendCmd(byte addr, DaliCmd value, byte type = 0, bool wait = false);
		uint8_t sendCmdSpecial(DaliSpecialCmd command, byte value = 0, bool wait = false);
		uint8_t sendArc(byte addr, byte value, byte type);
		void koHandleSwitch(GroupObject & ko);
		void koHandleDimm(GroupObject & ko);
		void koHandleDayNight(GroupObject & ko);
		void koHandleOnValue(GroupObject & ko);
		void koHandleScene(GroupObject & ko);

		void funcHandleType(uint8_t *data, uint8_t *resultData, uint8_t &resultLength);
		void funcHandleScan(uint8_t *data, uint8_t *resultData, uint8_t &resultLength);
		void funcHandleAssign(uint8_t *data, uint8_t *resultData, uint8_t &resultLength);
		void funcHandleAddress(uint8_t *data, uint8_t *resultData, uint8_t &resultLength);
		void funcHandleEvgWrite(uint8_t *data, uint8_t *resultData, uint8_t &resultLength);
		void funcHandleEvgRead(uint8_t *data, uint8_t *resultData, uint8_t &resultLength);

		void stateHandleType(uint8_t *data, uint8_t *resultData, uint8_t &resultLength);
		void stateHandleAssign(uint8_t *data, uint8_t *resultData, uint8_t &resultLength);
		void stateHandleScanAndAddress(uint8_t *data, uint8_t *resultData, uint8_t &resultLength);
};

extern DaliModule openknxDaliModule;
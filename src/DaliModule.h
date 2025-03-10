#pragma once

#include <Arduino.h>
#include "OpenKNX.h"
#include "dali/Master.h"
#include "dali/Commands.h"
#include "DaliChannel.h"
#include "Ballast.hpp"
#include "HclCurve.h"

#ifndef DALI_WAIT_RANDOMIZE
#define DALI_WAIT_RANDOMIZE 1000
#endif
#ifndef DALI_WAIT_SEARCH
#define DALI_WAIT_SEARCH 300
#endif

#define SCE_CountNumber 64

typedef void (*EventHandlerChangedGroupFuncPtr)(uint8_t index, uint8_t value);

class DaliModule : public OpenKNX::Module
{
	public:
		void loop(bool configured) override;
		void loop1(bool configured);
		void setup(bool conf) override;
		void setup1(bool conf);
		bool processCommand(const std::string cmd, bool diagnoseKo) override;
		void processInputKo(GroupObject &ko) override;
		void showHelp() override;

		const std::string name() override;
		const std::string version() override;

		bool getDaliBusState();

		bool processFunctionProperty(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t *data, uint8_t *resultData, uint8_t &resultLength) override;
		bool processFunctionPropertyState(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t *data, uint8_t *resultData, uint8_t &resultLength) override;
		
	private:
		enum class AddressingState {
			OFF,
			INIT,
			INIT2,
			WRITE_DTR,
			REMOVE_SHORT,
			REMOVE_SHORT2,
			RANDOM,
			RANDOMWAIT,
			STARTSEARCH,
			SEARCHHIGH,
			SEARCHMID,
			SEARCHLOW,
			COMPARE,
			GETSHORT,
			CHECKFOUND,
			PROGRAMSHORT,
			VERIFYSHORT,
			VERIFYSHORTRESPONSE,
			WITHDRAW,
			TERMINATE,
			SEARCHSHORT,
			CHECKSEARCHSHORT
		};
		enum class AssigningState {
			OFF,
			INIT,
			QUERY,
			CHECKQUERY,
			STARTSEARCH,
			COMPARE,
			CHECKFOUND,
			WITHDRAW,
			PROGRAMSHORT,
			VERIFYSHORT,
			VERIFYSHORTRESPONSE,
			TERMINATE
		};
		enum class AssigningResponse {
			SUCCESS,
			NOT_FREE,
			NO_RESPONSE,
			NO_RESPONSE_LONG,
			FAILED
		};
		
		void loopAddressing();
		void loopAssigning();
		void loopBusState();
		void loopInitData();
		void loopGroupState();
#ifdef INFO2_LED_PIN
		void loopError();
#endif
#ifdef FUNC1_BUTTON_PIN
		void handleFunc(uint8_t setting);
		bool _currentToggleState = false;
		uint8_t _currentIdentifyDevice = 0;
#endif
		bool _currentLockState = false;
		int16_t getInfo(byte address, uint8_t command, uint8_t additional = 0);
	
		AddressingState _adrState = AddressingState::OFF;
		AssigningState _assState = AssigningState::OFF;
		AssigningResponse _assResponse = AssigningResponse::SUCCESS;
		Ballast ballasts[64];
		bool addresses[64];
		int _adrFound = 0;
		uint8_t _adrNew = 0;
		uint8_t _lastBusState = 2;
		byte _adrIterations;
		unsigned long _adrSearch;
		bool _adrAssign = false;
		bool _adrOnlyNew = false;
		bool _adrRandomize = false;
		bool _adrDeleteAll = false;
		uint32_t _adrResponse = 0;


		uint8_t _lastChangedGroup = 255;
		uint8_t _lastChangedValue = 0;

		bool _gotInitData = false;
		bool _daliBusState = true;
		bool _daliBusStateToSet = true;
		unsigned long _daliStateLast = 1;
		Dali::Master daliMaster;
		DaliChannel channels[64] {daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster};
		DaliChannel groups[16] {daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster,daliMaster};
		HclCurve curves[3];
		#ifdef DALI_NO_TIMER
		struct repeating_timer _timer;
		#endif

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
		void funcHandleSetScene(uint8_t *data, uint8_t *resultData, uint8_t &resultLength);
		void funcHandleGetScene(uint8_t *data, uint8_t *resultData, uint8_t &resultLength);
		void funcHandleIdentify(uint8_t *data, uint8_t *resultData, uint8_t &resultLength);

		void cmdHandleScan(bool hasArg, std::string arg);
		void cmdHandleArc(bool hasArg, std::string arg);
		void cmdHandleSet(bool hasArg, std::string arg);
		void cmdHandleAuto(bool hasArg, std::string arg);
		void cmdHandleStepUp(bool hasArg, std::string arg);
		void cmdHandleStepDown(bool hasArg, std::string arg);
		void cmdHandleGetLvl(bool hasArg, std::string arg);

		void stateHandleAssign(uint8_t *data, uint8_t *resultData, uint8_t &resultLength);
		void stateHandleScanAndAddress(uint8_t *data, uint8_t *resultData, uint8_t &resultLength);
		void stateHandleFoundEVGs(uint8_t *data, uint8_t *resultData, uint8_t &resultLength);
};

extern DaliModule openknxDaliModule;
#include <Arduino.h>
#include "OpenKNX.h"
#include "Dali.h"
#include "StandardChannel.h"
#include "StaircaseChannel.h"

class DaliModule : public OpenKNX::Module
{
	public:
		void loop() override;
		void loop1() override;
		void setup() override;
		bool usesDualCore() override;
		void processInputKo(GroupObject &ko) override;

		const std::string name() override;
		const std::string version() override;
		// void writeFlash() override;
		// void readFlash(const uint8_t* data, const uint16_t size) override;
		// uint16_t flashSize() override;

		bool processFunctionProperty(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t *data, uint8_t *resultData, uint8_t &resultLength) override;
		bool processFunctionPropertyState(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t *data, uint8_t *resultData, uint8_t &resultLength) override;

	private:
		DaliClass *dali;
		OpenKNX::Channel *channels[64];
		//StandardChannel **standards;
		//StaircaseChannel **staircases;
		//int standardCount = 0;
		//int staircaseCount = 0;
};
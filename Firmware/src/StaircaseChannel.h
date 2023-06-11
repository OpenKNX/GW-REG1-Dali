#include "OpenKNX.h"
#include "Dali.h"

class StaircaseChannel : public OpenKNX::Channel
{
	public:
        StaircaseChannel(uint8_t channelIndex, DaliClass *dali);
        ~StaircaseChannel();

		void loop() override;
		void loop1() override;
		void setup() override;
		void processInputKo(GroupObject &ko) override;

		const std::string name() override;
		// void writeFlash() override;
		// void readFlash(const uint8_t* data, const uint16_t size) override;
		// uint16_t flashSize() override;

	private:
		DaliClass *_dali;
};
#include "OpenKNX.h"
#include "Dali.h"
#include "MessageQueue.h"

class StaircaseChannel : public OpenKNX::Channel
{
	public:
        StaircaseChannel(uint8_t channelIndex, MessageQueue *queue, bool isGroup);
        ~StaircaseChannel();

		void loop() override;
		void loop1() override;
		//void setup() override;
		void processInputKo(GroupObject &ko) override;
		void setInterval(uint i);
		void setMin(uint8_t min);
		void setMax(uint8_t max);

		const std::string name() override;
		// void writeFlash() override;
		// void readFlash(const uint8_t* data, const uint16_t size) override;
		// uint16_t flashSize() override;

	private:
		MessageQueue *_queue;
		bool state = false;
		bool isGroup = false;
		bool isLocked = false;
		unsigned long startTime = 0;
		uint interval = 0;
		uint8_t _min = 0;
		uint8_t _max = 0;

		uint8_t sendMsg(MessageType t, byte value);
};
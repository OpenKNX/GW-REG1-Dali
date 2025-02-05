#pragma once

#include "Arduino.h"
#include "Message.hpp"

#ifdef ARDUINO_ARCH_ESP32
#else
#include <mutex>
#endif

class MessageQueue
{
	public:
        void init();
        uint8_t push(Message *msg);
        bool pop(Message &msg);
        uint8_t getNextId();
        void setResponse(uint8_t id, int16_t value);
        int16_t getResponse(uint8_t id);

    private:
        Message *head;
        Message *tail;
        uint8_t currentId = 0;
		int16_t responses[256];
        unsigned long lastPush = 0;
        unsigned long lastPop = 0;

        #ifdef ARDUINO_ARCH_ESP32
        SemaphoreHandle_t mutex_handle;
        #else
        mutex_t mutex;
        #endif
};
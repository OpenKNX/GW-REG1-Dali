#pragma once

#include "Arduino.h"
#include "Message.hpp"

class MessageQueue
{
	public:
        uint8_t push(Message *msg);
        Message* pop();
        uint8_t getNextId();
        void setResponse(uint8_t id, int16_t value);
        int16_t getResponse(uint8_t id);

    private:
        Message *head;
        Message *tail;
        uint8_t currentId = 0;
		int16_t *responses = new int16_t[256];
        unsigned long lastPush = 0;
        bool isLocked = false;
};
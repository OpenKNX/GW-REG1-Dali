#pragma once

#include "Arduino.h"

enum class MessageType
{
    Arc,
    Cmd
};

struct Message 
{
    Message *next;
    byte *data;
    MessageType type;
    byte addr;
    byte addrtype = 0;
    byte value;
    bool wait = false;
    uint8_t id = 0;
};

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
        uint8_t currentId;
		int16_t *responses = new int16_t[256];
};
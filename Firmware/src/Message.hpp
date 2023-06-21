#pragma once
#include <Arduino.h>

enum class MessageType
{
    Arc,
    Cmd,
    SpecialCmd
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

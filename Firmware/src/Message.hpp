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
    byte para1;
    byte addrtype = 0;
    byte para2;
    bool wait = false;
    uint8_t id = 0;
};

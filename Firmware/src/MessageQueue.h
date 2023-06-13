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

    private:
        Message *start;
        Message *end;
        uint8_t currentId;
};
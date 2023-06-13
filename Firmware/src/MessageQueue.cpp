#include "Arduino.h"
#include "MessageQueue.h"

uint8_t MessageQueue::push(Message *msg)
{
    msg->id = currentId++;
    if(end == nullptr)
    {
        start = msg;
        end = msg;
        return msg->id;
    }

    end->next = msg;
    end = msg;
    return msg->id;
}

Message* MessageQueue::pop()
{
    if(start == nullptr) return nullptr;

    Message *item = start;
    
    if(item->next == nullptr)
    {
        start = nullptr;
        end = nullptr;
    } else {
        start = item->next;
    }

    return item;
}
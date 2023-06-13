#include "Arduino.h"
#include "MessageQueue.h"

uint8_t MessageQueue::push(Message *msg)
{
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

uint8_t MessageQueue::getNextId()
{
    responses[currentId] = -255;
    return currentId++;
}

void MessageQueue::setResponse(uint8_t id, int16_t value)
{
    responses[id] = value;
}

int16_t MessageQueue::getResponse(uint8_t id)
{
    return responses[id];
}
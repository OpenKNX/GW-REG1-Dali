#include "Arduino.h"
#include "MessageQueue.h"

uint8_t MessageQueue::push(Message *msg)
{
    msg->next = nullptr;
    if(tail == nullptr)
    {
        head = msg;
        tail = msg;
        return msg->id;
    }

    tail->next = msg;
    tail = msg;
    return msg->id;
}

Message* MessageQueue::pop()
{
    if(start == nullptr) return nullptr;

    Message *item = head;
    
    if(item->next == nullptr)
    {
        head = nullptr;
        tail = nullptr;
    } else {
        head = item->next;
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
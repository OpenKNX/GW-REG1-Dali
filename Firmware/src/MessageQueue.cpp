#include "Arduino.h"
#include "OpenKNX.h"
#include "MessageQueue.h"

uint8_t MessageQueue::push(Message *msg)
{
    while(isLocked) ;
    isLocked = true;
    lastPush = millis();

    msg->next = nullptr;
    if(tail == nullptr)
    {
        head = msg;
        tail = msg;
        isLocked = false;
        return msg->id;
    }

    tail->next = msg;
    tail = msg;
    isLocked = false;
    return msg->id;
}

Message* MessageQueue::pop()
{
    if(millis() - lastPush < 3) return nullptr;
    while(isLocked) ;

    if(head == nullptr) return nullptr;
    isLocked = true;

    Message *item = head;
    
    if(item->next == nullptr)
    {
        head = nullptr;
        tail = nullptr;
    } else {
        head = item->next;
    }

    isLocked = false;
    return item;
}

uint8_t MessageQueue::getNextId()
{
    responses[currentId] = -200;
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
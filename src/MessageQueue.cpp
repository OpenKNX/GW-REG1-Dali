#include "Arduino.h"
#include "OpenKNX.h"
#include "MessageQueue.h"

void MessageQueue::init()
{
    //queue_handle = xQueueCreate(10, sizeof(Message));
    #ifdef ARDUINO_ARCH_ESP32
    mutex_handle = xSemaphoreCreateMutex();
    #else
    mutex_init(&mutex);
    #endif
}

uint8_t MessageQueue::push(Message *msg)
{
#ifdef ARDUINO_ARCH_ESP32
    xQueueSemaphoreTake(mutex_handle, portMAX_DELAY);
#else
    if(!mutex_try_enter_block_until(&mutex, 1000))
    {
        logError("Queue", "Mutex timeout");
        return -1;
    }
#endif

    msg->next = nullptr;
    if(tail == nullptr)
    {
        head = msg;
        tail = msg;
#ifdef ARDUINO_ARCH_ESP32
        xSemaphoreGive(mutex_handle);
#else
        mutex_exit(&mutex);
#endif
        return msg->id;
    }

    tail->next = msg;
    tail = msg;
    lastPush = millis();
    
#ifdef ARDUINO_ARCH_ESP32
    xSemaphoreGive(mutex_handle);
#else
    mutex_exit(&mutex);
#endif
    return msg->id;
}

bool MessageQueue::pop(Message &msg)
{
    if(head == nullptr) return false;
    if(millis() - lastPush < 50) return false;
    if(lastPop != 0 && (millis() - lastPop < 200)) return false;

#ifdef ARDUINO_ARCH_ESP32
    xSemaphoreTake(mutex_handle, portMAX_DELAY);
#else
    if(!mutex_try_enter_block_until(&mutex, 1000))
    {
        logError("Queue", "Mutex timeout");
        return false;
    }
#endif

    if(head == nullptr)
    {
#ifdef ARDUINO_ARCH_ESP32
        xSemaphoreGive(mutex_handle);
#else
        mutex_exit(&mutex);
#endif
        return false;
    }
    
    if(head == nullptr)
        lastPop = millis();
    else
        lastPop = 0;

    msg.addrtype = head->addrtype;
    msg.id = head->id;
    msg.para1 = head->para1;
    msg.para2 = head->para2;
    msg.type = head->type;
    msg.wait = head->wait;

    Message *temp = head;

    if(head->next == nullptr)
    {
        head = nullptr;
        tail = nullptr;
    } else {
        head = head->next;
    }

    delete temp;

#ifdef ARDUINO_ARCH_ESP32
    xSemaphoreGive(mutex_handle);
#else
    mutex_exit(&mutex);
#endif
    return true;
}

uint8_t MessageQueue::getNextId()
{
    currentId++;
    if(currentId == 0) currentId++;
    responses[currentId] = -200;
    return currentId;
}

void MessageQueue::setResponse(uint8_t id, int16_t value)
{
    responses[id] = value;
}

int16_t MessageQueue::getResponse(uint8_t id)
{
    if(id == 0) return -13;
    return responses[id];
}
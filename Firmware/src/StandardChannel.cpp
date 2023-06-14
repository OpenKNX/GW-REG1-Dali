#include "StandardChannel.h"

StandardChannel::StandardChannel(uint8_t channelIndex, MessageQueue *queue, bool ig)
{
    _channelIndex = channelIndex;
    _queue = queue;
    isGroup = ig;
}

StandardChannel::~StandardChannel()
{
}

const std::string StandardChannel::name()
{
    return "StandardChannel";
}


//will be called once
//only if knx.configured == true
void StandardChannel::setup()
{
}

void StandardChannel::loop()
{
}

void StandardChannel::loop1()
{
}

void StandardChannel::processInputKo(GroupObject &ko)
{
    int chanIndex = (ko.asap() - ADR_KoOffset) % ADR_KoBlockSize;
    logInfoP("Got KO %i", chanIndex);
    switch(chanIndex)
    {
        case 0:
        {
            if(ko.value(DPT_Switch))
            {
                logInfoP("Einschalten");
                Message *msg = new Message();
                msg->id = _queue->getNextId();
                msg->type = MessageType::Arc;
                msg->addr = _channelIndex;
                msg->value = 0xFE;
                _queue->push(msg);
            }
            else
            {
                logInfoP("Ausschalten");
                Message *msg = new Message();
                msg->id = _queue->getNextId();
                msg->type = MessageType::Arc;
                msg->addr = _channelIndex;
                msg->value = 0x00;
                _queue->push(msg);
            }
            break;
        }

        case 2:
        {
            logInfoP("Dimmen relativ");
            break;
        }

        case 3:
        {
            logInfoP("Dimmen Absolut auf %i%%", ko.value(Dpt(5,1)));
            Message *msg = new Message();
                msg->id = _queue->getNextId();
            msg->type = MessageType::Arc;
            msg->addr = _channelIndex;
            msg->value = ko.value(Dpt(5,1));
            _queue->push(msg);
            break;
        }
    }
}
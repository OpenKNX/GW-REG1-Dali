#include "StaircaseChannel.h"

StaircaseChannel::StaircaseChannel(uint8_t channelIndex, MessageQueue *queue, bool ig)
{
    _channelIndex = channelIndex;
    _queue = queue;
    isGroup = ig;
}

StaircaseChannel::~StaircaseChannel()
{
}

const std::string StaircaseChannel::name()
{
    return "StaircaseChannel";
}


//will be called once
//only if knx.configured == true
void StaircaseChannel::setup()
{
    interval = ParamADR_stairtime;
}

void StaircaseChannel::loop()
{
    if(state)
    {
        if(millis() - startTime > interval)
        {
            state = false;
            knx.getGroupObject(ADR_Koswitch_state).value(false, DPT_Switch);
            sendMsg(MessageType::Arc, 0xFE);
        }
    }
}

void StaircaseChannel::loop1()
{
}

uint8_t StaircaseChannel::sendMsg(MessageType t, byte v)
{
    Message *msg = new Message();
    msg->type = t;
    msg->addr = _channelIndex;
    msg->addrtype = isGroup;
    msg->value = 0xFE;
    return _queue->push(msg);
}

void StaircaseChannel::processInputKo(GroupObject &ko)
{
    int chanIndex = (ko.asap() - ADR_KoOffset) % ADR_KoBlockSize;
    logInfoP("Got KO %i", chanIndex);

    switch(chanIndex)
    {
        case 0:
        {
            if(ko.value(DPT_Switch))
            {
                if(state)
                {
                    if(ParamADR_nachtriggern)
                    {
                        startTime = millis();
                        return;
                    } else {
                        return;
                    }
                }
                logInfoP("Einschalten");
                state = true;
                startTime = millis();
                sendMsg(MessageType::Arc, 0xFE);
                knx.getGroupObject(ADR_Koswitch_state).value(true, DPT_Switch);
            }
            else
            {
                bool manuOff = ParamADR_manuoff;
                if(!manuOff) return;

                logInfoP("Ausschalten");
                sendMsg(MessageType::Arc, 0x00);
                knx.getGroupObject(ADR_Koswitch_state).value(false, DPT_Switch);
                state = false;
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
            sendMsg(MessageType::Arc, ko.value(Dpt(5,1)));
            break;
        }
    }
}
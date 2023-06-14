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
    _min = (ParamADR_min * 2.54);
    _max = (ParamADR_max * 2.54);
}

void StaircaseChannel::loop()
{
    if(state)
    {
        if(millis() - startTime > interval * 1000)
        {
            logInfoP("Zeit abgelaufen");
            state = false;
            //knx.getGroupObject(ADR_Koswitch_state).value(false, DPT_Switch);
            sendMsg(MessageType::Arc, 0x00);
        }
    }
}

void StaircaseChannel::loop1()
{
}

uint8_t StaircaseChannel::sendMsg(MessageType t, byte v)
{
    Message *msg = new Message();
    msg->id = _queue->getNextId();
    msg->type = t;
    msg->addr = _channelIndex;
    msg->addrtype = isGroup;
    msg->value = v;
    return _queue->push(msg);
}

void StaircaseChannel::processInputKo(GroupObject &ko)
{
    int chanIndex = (ko.asap() - ADR_KoOffset) % ADR_KoBlockSize;
    logInfoP("Got KO %i", chanIndex);

    switch(chanIndex)
    {
        //Schalten
        case 0:
        {
            if(isLocked)
            {
                logInfoP("is locked");
                return;
            }

            if(ko.value(DPT_Switch))
            {
                
                if(state)
                {
                    logInfoP("ist bereits an");
                    if(ParamADR_nachtriggern)
                    {
                        logInfoP("wurde nachgetriggert");
                        startTime = millis();
                        return;
                    }
                    return;
                }
                logInfoP("Einschalten");
                state = true;
                startTime = millis();
                logInfoP("interval %i", interval);
                sendMsg(MessageType::Arc, 0xFE);
                //knx.getGroupObject(ADR_Koswitch_state).value(true, DPT_Switch);
            }
            else
            {
                bool manuOff = ParamADR_manuoff;
                if(!manuOff)
                {
                    logInfoP("no manuel off");
                    return;
                }

                logInfoP("Ausschalten");
                sendMsg(MessageType::Arc, 0x00);
                //knx.getGroupObject(ADR_Koswitch_state).value(false, DPT_Switch);
                state = false;
            }
            break;
        }

        //Schalten Status
        //case 1

        //Dimmen relativ
        case 2:
        {
            if(isLocked) break;

            logInfoP("Dimmen relativ");
            break;
        }

        //Dimmen Absolut
        case 3:
        {
            if(isLocked) break;

            uint8_t value = ko.value(Dpt(5,1));
            logInfoP("Dimmen Absolut auf %i%%", value);
            uint val = _min + ((_max - _min) * (value / 100));
            //Pvalue = 10 ^ ((value-1) / (253/3)) * Pmax / 1000
            logInfoP("DALI Wert: %i", val);
            sendMsg(MessageType::Arc, ko.value(Dpt(5,1)));
            break;
        }

        //Dimmen Status
        //case 4

        //Sperren
        case 5:
        {
            bool value = ko.value(Dpt(1,5));
            if(isLocked == value) break;
            isLocked = value;
            uint8_t behave;
            uint8_t behavevalue;
            if(isLocked)
            {
                behave = ParamADR_lockbehave;
                behavevalue = ParamADR_lockvalue * 2.54;
            } else {
                behave = ParamADR_unlockbehave;
                behavevalue = ParamADR_unlockvalue * 2.54;
            }
            switch(behave)
            {
                //nothing
                case 0:
                    return;

                //Ausschalten
                case 1:
                {
                    behavevalue = 0;
                    break;
                }

                //Einschalten
                case 2:
                {
                    behavevalue = 254;
                    break;
                }

                //Fester Wert
                case 3:
                {
                    break;
                }
            }
            sendMsg(MessageType::Arc, behavevalue);
            break;
        }
    }
}
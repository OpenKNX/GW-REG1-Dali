#include "DaliChannel.h"

DaliChannel::DaliChannel(uint8_t channelIndex, MessageQueue *queue, bool ig)
{
    _channelIndex = channelIndex;
    _queue = queue;
    isGroup = ig;
}

DaliChannel::~DaliChannel()
{
}

const std::string DaliChannel::name()
{
    if(_isStaircase)
        if(isGroup)
            return "DaliChannel_G";
        else
            return "DaliChannel_A";

            
    if(isGroup)
        return "StandardChannel_G";
    return "StandardChannel_A";
}


//will be called once
//only if knx.configured == true
void DaliChannel::setup()
{
    if(isGroup)
    {
        _isStaircase = ParamGRP_type;
        _min = 0;
        _max = 0xFF;
        if(_isStaircase)
            interval = ParamGRP_stairtime;
    } else {
        _isStaircase = ParamADR_type;
        _min = ParamADR_min * 2.54;
        _max = ParamADR_max * 2.54;
        if(_isStaircase)
            interval = ParamADR_stairtime;
    }
    logInfoP("Setup");
}


void DaliChannel::loop()
{
    if(_isStaircase && state)
    {
        if(millis() - startTime > interval * 1000)
        {
            logInfoP("Zeit abgelaufen");
            state = false;
            if(isGroup)
                knx.getGroupObject(calcKoNumber(ADR_Koswitch_state)).value(false, DPT_Switch);
            else
                knx.getGroupObject(calcKoNumber(GRP_Koswitch_state)).value(false, DPT_Switch);
            sendArc(0x00);
        }
    }
}

void DaliChannel::loop1()
{
}

uint16_t DaliChannel::calcKoNumber(int asap)
{
    if(isGroup)
        return asap + (GRP_KoBlockSize * _channelIndex) + GRP_KoOffset;
    
    return asap + (ADR_KoBlockSize * _channelIndex) + ADR_KoOffset;
}

uint8_t DaliChannel::sendArc(byte v)
{
    Message *msg = new Message();
    msg->id = _queue->getNextId();
    msg->type = MessageType::Arc;
    msg->para1 = _channelIndex;
    msg->para2 = v;
    msg->addrtype = isGroup;
    return _queue->push(msg);
}

void DaliChannel::processInputKo(GroupObject &ko)
{
    int chanIndex = 0;
    if(isGroup)
    {
        chanIndex = (ko.asap() - GRP_KoOffset) % GRP_KoBlockSize;
        logInfoP("Got KO %i", chanIndex);
    } else {
        chanIndex = (ko.asap() - ADR_KoOffset) % ADR_KoBlockSize;
        logInfoP("Got KO %i", chanIndex);
    }

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

            if(_isStaircase)
            {
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
                    sendArc(0xFE);
                    if(isGroup)
                        knx.getGroupObject(calcKoNumber(ADR_Koswitch_state)).value(false, DPT_Switch);
                    else
                        knx.getGroupObject(calcKoNumber(GRP_Koswitch_state)).value(false, DPT_Switch);
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
                    sendArc(0x00);
                    if(isGroup)
                        knx.getGroupObject(calcKoNumber(ADR_Koswitch_state)).value(false, DPT_Switch);
                    else
                        knx.getGroupObject(calcKoNumber(GRP_Koswitch_state)).value(false, DPT_Switch);
                    state = false;
                }
            } else {
                if(isGroup)
                    knx.getGroupObject(calcKoNumber(ADR_Koswitch_state)).value(ko.value(DPT_Switch), DPT_Switch);
                else
                    knx.getGroupObject(calcKoNumber(GRP_Koswitch_state)).value(ko.value(DPT_Switch), DPT_Switch);
            }
            break;
        }

        //Schalten Status
        //case 1

        //Dimmen relativ
        case 2:
        {
            if(isLocked)
            {
                logInfoP("is locked");
                return;
            }

            logInfoP("Dimmen relativ");
            break;
        }

        //Dimmen Absolut
        case 3:
        {
            if(isLocked)
            {
                logInfoP("is locked");
                return;
            }

            uint8_t value = ko.value(Dpt(5,1));
            logInfoP("Dimmen Absolut auf %i%%", value);
            uint val = _min + ((_max - _min) * (value / 100));
            //Pvalue = 10 ^ ((value-1) / (253/3)) * Pmax / 1000
            logInfoP("DALI Wert: %i", val);
            sendArc(val);
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
            sendArc(behavevalue);
            break;
        }
    }
}
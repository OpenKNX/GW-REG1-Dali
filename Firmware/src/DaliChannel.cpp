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
            return "StaircaseChannel_G";
        else
            return "StaircaseChannel_A";

            
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
        _isConfigured = ParamGRP_deviceType != 0;
        if(!_isConfigured)
        {
            logInfoP("Nicht konfiguriert");
            return;
        }

        _isStaircase = ParamGRP_type;
        _min = 0;
        _max = 0xFF;
        if(_isStaircase)
            interval = ParamGRP_stairtime;
        _onDay = ParamGRP_onDay;
        _onNight = ParamGRP_onNight;
    } else {
        _isConfigured = ParamADR_deviceType != 15;
        if(!_isConfigured)
        {
            logInfoP("Nicht konfiguriert");
            return;
        }

        _isStaircase = ParamADR_type;
        _min = ParamADR_min;
        _max = ParamADR_max;
        if(_isStaircase)
            interval = ParamADR_stairtime;
        _onDay = ParamADR_onDay;
        _onNight = ParamADR_onNight;
    }
    logInfoP("Min/Max: %i/%i | Day/Night: %i/%i | %is", _min, _max, _onDay, _onNight, interval);
}


void DaliChannel::loop()
{
    if(_isStaircase && state)
    {
        if(millis() - startTime > interval * 1000)
        {
            logInfoP("Zeit abgelaufen");
            state = false;
            sendArc(0x00);
            setSwitchState(false);
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
    msg->para2 = percentToArc(v);
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
        case ADR_Koswitch:
        {
            logInfoP("Schalten");
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
                    logInfoP(isNight ? "Einschalten Nacht" : "Einschalten Tag");
                    state = true;
                    startTime = millis();
                    logInfoP("interval %i", interval);
                    sendArc(isNight ? _onNight : _onDay);
                    setSwitchState(true);
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
                    setSwitchState(false);
                    state = false;
                }
            } else {
                bool value = ko.value(DPT_Switch);
                if(value)
                {
                    logInfoP(isNight ? "Einschalten Nacht" : "Einschalten Tag");
                    sendArc(isNight ? _onNight : _onDay);
                }
                else {
                    logInfoP(isNight ? "Ausschalten Nacht" : "Ausschalten Tag");
                    sendArc(0x00);
                }
                
                setSwitchState(value);
            }
            break;
        }

        //Schalten Status
        //case 1

        //Dimmen relativ
        case ADR_Kodimm_relative:
        {
            logInfoP("Dimmen relativ");
            if(isLocked)
            {
                logInfoP("is locked");
                return;
            }

            break;
        }

        //Dimmen Absolut
        case ADR_Kodimm_absolute:
        {
            logInfoP("Dimmen absolut");
            if(isLocked)
            {
                logInfoP("is locked");
                return;
            }

            uint8_t value = ko.value(Dpt(5,1));
            logInfoP("Dimmen Absolut auf %i%%", value);
            logInfoP("DALI Wert: %i", percentToArc(value));
            sendArc(value);
            setSwitchState(value > 0);
            setDimmState(value);
            break;
        }

        //Dimmen Status
        //case 4

        //Sperren
        case ADR_Kolock:
        {
            bool value = ko.value(Dpt(1,1));

            if(isGroup)
            {
                if(ParamGRP_locknegate)
                    value = !value;
            } else {
                if(ParamADR_locknegate)
                    value = !value;
            }

            if(isLocked == value) break;
            isLocked = value;
            uint8_t behave;
            uint8_t behavevalue;
            if(isLocked)
            {
                logInfoP("Sperren");
                behave = ParamADR_lockbehave;
                behavevalue = ParamADR_lockvalue;
            } else {
                logInfoP("Entsperren");
                behave = ParamADR_unlockbehave;
                behavevalue = ParamADR_unlockvalue;
            }

            switch(behave)
            {
                //nothing
                case 0:
                    logInfoP("Nichts");
                    return;

                //Ausschalten
                case 1:
                {
                    logInfoP("Ein");
                    behavevalue = isNight ? _onNight : _onDay;
                    break;
                }

                //Einschalten
                case 2:
                {
                    logInfoP("Aus");
                    behavevalue = 0;
                    break;
                }

                //Fester Wert
                case 3:
                {
                    logInfoP("Wert");
                    break;
                }
            }
            logInfoP("%i - %i", behavevalue, percentToArc(behavevalue));
            sendArc(behavevalue);
            setSwitchState(behavevalue > 0);
            setDimmState(behavevalue);
            break;
        }
    }
}

uint8_t DaliChannel::percentToArc(uint8_t value)
{
    if(value == 0) return 0;
    //Todo also include _max
    uint8_t arc = ((253/3)*(log10(value)+1)) + 1;
    arc++;
    return arc;
}

void DaliChannel::setSwitchState(bool value)
{
    if(value == _lastState) return;
    _lastState = value;

    if(isGroup)
        knx.getGroupObject(calcKoNumber(ADR_Koswitch_state)).value(value, DPT_Switch);
    else
        knx.getGroupObject(calcKoNumber(GRP_Koswitch_state)).value(value, DPT_Switch);
}

void DaliChannel::setDimmState(uint8_t value)
{
    if(value == _lastValue) return;
    _lastValue = value;

    if(isGroup)
        knx.getGroupObject(calcKoNumber(ADR_Kodimm_state)).value(value, Dpt(5, 1));
    else
        knx.getGroupObject(calcKoNumber(GRP_Kodimm_state)).value(value, Dpt(5, 1));
}

void DaliChannel::setOnValue(uint8_t value)
{
    _onDay = value;
}
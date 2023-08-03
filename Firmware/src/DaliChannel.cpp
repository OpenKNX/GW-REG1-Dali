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
        _getError = ParamADR_error;
    }
    logInfoP("Min/Max: %i/%i | Day/Night: %i/%i | %is", _min, _max, _onDay, _onNight, interval);
}


void DaliChannel::loop()
{
}

void DaliChannel::loop1()
{
    if(!_isConfigured) return;

    if(_isStaircase && state)
    {
        if(millis() - startTime > interval * 1000)
        {
            logInfoP("Zeit abgelaufen");
            state = false;
            sendArc(0x00);
            setSwitchState(false);
            setDimmState(0);
        }
    }

    if(_dimmDirection != DimmDirection::None && millis() - _dimmLast > DimmInterval)
    {
        if(_dimmDirection == DimmDirection::Up)
        {
            logInfoP("Dimm Up");
            sendCmd(8); //STEP_UP_AND_ON
            _lastStep--;
        } else if(_dimmDirection == DimmDirection::Down) {
            logInfoP("Dimm Down");
            sendCmd(7); //STEP_DOWN_AND_OFF
            _lastStep++;
        }

        if(_lastStep == 0)
        {
            logInfoP("Dimm Stop at 0");
            _dimmDirection = DimmDirection::None;
        } else if(_lastStep == 254) {
            logInfoP("Dimm Stop at 254");
            _dimmDirection = DimmDirection::None;
        }

        setSwitchState(_lastStep > 0);
        setDimmState(arcToPercent(_lastStep));

        _dimmLast = millis();
    }

    if(!isGroup && _getError)
    {
        if(_errorResp != 300)
        {
            int8_t resp = _queue->getResponse(_errorResp);
            if(resp == -200) return;
            _errorResp = 300;
            if(resp == -1) resp = 1;
            if(resp < 0) return;

            resp = resp & 0b11;
            bool val = knx.getGroupObject(calcKoNumber(ADR_Koerror)).value(Dpt(1,0));
            if(val != resp)
                knx.getGroupObject(calcKoNumber(ADR_Koerror)).value(val != 0);
        }

        if(millis() - _lastError > 60000)
        {
            _errorResp = sendCmd(144); //QUERY_STATUS
            _lastError = millis();
        }
    }
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

uint8_t DaliChannel::sendCmd(byte cmd)
{
    Message *msg = new Message();
    msg->id = _queue->getNextId();
    msg->type = MessageType::Cmd;
    msg->para1 = _channelIndex;
    msg->para2 = cmd;
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

                    uint8_t onValue = isNight ? _onNight : _onDay;
                    if(onValue == 0) onValue = isNight ? _lastNightValue : _lastDayValue;
                    sendArc(onValue);
                    setDimmState(onValue);
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
                    setDimmState(0);
                    state = false;
                }
            } else {
                bool value = ko.value(DPT_Switch);
                if(value)
                {
                    uint8_t onValue = isNight ? _onNight : _onDay;
                    if(onValue == 0) onValue = isNight ? _lastNightValue : _lastDayValue;
                    logInfoP(isNight ? "Einschalten Nacht" : "Einschalten Tag");
                    sendArc(onValue);
                    setDimmState(onValue);
                }
                else {
                    logInfoP("Ausschalten");
                    sendArc(0x00);
                    setDimmState(0);
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

            _dimmStep = ko.value(Dpt(3,7,1));

            if(_dimmStep == 0)
            {
                logInfoP("Dimm Stop");
                _dimmDirection = DimmDirection::None;
                _dimmLast = 0;
                setSwitchState(_lastStep > 0);
                setDimmState(arcToPercent(_lastStep));
                return;
            }

            _dimmDirection = ko.value(Dpt(3,7,0)) ? DimmDirection::Up : DimmDirection::Down;
            if(_dimmDirection == DimmDirection::Up)
            {
                logInfoP("Dimm Up Start");
            } else if(_dimmDirection == DimmDirection::Down) {
                logInfoP("Dimm Down Start");
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
    if(value == 0)
    {
        _lastState = 0;
        return 0;
    }
    //Todo also include _max
    uint8_t arc = ((253/3)*(log10(value)+1)) + 1;
    _lastState = arc;
    return arc;
}

uint8_t DaliChannel::arcToPercent(uint8_t value)
{
    if(value == 0)
    {
        _lastState = 0;
        return 0;
    }
    //Todo also include _max
    uint8_t arc = pow(10, ((value-1) / (253/3) - 1));
    _lastState = arc;
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
    if(_dimmDirection == DimmDirection::None && value > 0)
    {
        if(isNight)
            _lastNightValue = value;
        else
            _lastDayValue = value;
    }

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
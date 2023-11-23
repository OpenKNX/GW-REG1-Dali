#include "DaliChannel.h"
#include "OpenKNX.h"

DaliChannel::DaliChannel(uint8_t channelIndex, MessageQueue *queue, bool ig)
{
    _channelIndex = channelIndex;
    _queue = queue;
    _isGroup = ig;
}

DaliChannel::~DaliChannel()
{
}

const std::string DaliChannel::name()
{
    if(_isStaircase)
        if(_isGroup)
            return "StaircaseChannel_G";
        else
            return "StaircaseChannel_A";

            
    if(_isGroup)
        return "StandardChannel_G";
    return "StandardChannel_A";
}

const bool DaliChannel::isConfigured()
{
    return _isConfigured;
}

const bool DaliChannel::isGroup()
{
    return _isGroup;
}


//will be called once
//only if knx.configured == true
void DaliChannel::setup()
{
    if(_isGroup)
    {
        _isConfigured = ParamGRP_deviceType != 0;
        if(!_isConfigured)
        {
            logDebugP("Nicht konfiguriert");
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
            logDebugP("Nicht konfiguriert");
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
    logDebugP("Min/Max: %i/%i | Day/Night: %i/%i | %is", _min, _max, _onDay, _onNight, interval);
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
            logDebugP("Zeit abgelaufen");
            state = false;
            sendArc(0x00);
            setSwitchState(false);
        }
    }

    if(_dimmDirection != DimmDirection::None && millis() - _dimmLast > DimmInterval)
    {
        if(_dimmDirection == DimmDirection::Up)
        {
            sendCmd(8); //STEP_UP_AND_ON
            _lastStep++;
        } else if(_dimmDirection == DimmDirection::Down) {
            sendCmd(7); //STEP_DOWN_AND_OFF
            _lastStep--;
        }
        
        if(_lastStep == 0)
        {
            logDebugP("Dimm Stop at 0");
            _dimmDirection = DimmDirection::None;
        } else if(_lastStep == 254) {
            logDebugP("Dimm Stop at 254");
            _dimmDirection = DimmDirection::None;
        }

        setDimmState(_lastStep);

        _dimmLast = millis();
    }

    if(!_isGroup && _getError)
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
                knx.getGroupObject(calcKoNumber(ADR_Koerror)).value((val != 0), DPT_Switch);
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
    if(_isGroup)
        return asap + (GRP_KoBlockSize * _channelIndex) + GRP_KoOffset;
    
    return asap + (ADR_KoBlockSize * _channelIndex) + ADR_KoOffset;
}

uint8_t DaliChannel::sendArc(byte v)
{
    Message *msg = new Message();
    msg->id = _queue->getNextId();
    msg->type = MessageType::Arc;
    msg->para1 = _channelIndex;
    msg->para2 = DaliHelper::percentToArc(v);
    msg->addrtype = _isGroup;
    return _queue->push(msg);
}

uint8_t DaliChannel::sendCmd(byte cmd)
{
    Message *msg = new Message();
    msg->id = _queue->getNextId();
    msg->type = MessageType::Cmd;
    msg->para1 = _channelIndex;
    msg->para2 = cmd;
    msg->addrtype = _isGroup;
    return _queue->push(msg);
}

uint8_t DaliChannel::sendSpecialCmd(DaliSpecialCmd cmd, byte value)
{
    Message *msg = new Message();
    msg->id = _queue->getNextId();
    msg->type = MessageType::SpecialCmd;
    msg->para1 = static_cast<uint8_t>(cmd);
    msg->para2 = value;
    msg->addrtype = _isGroup;
    return _queue->push(msg);
}

void DaliChannel::processInputKo(GroupObject &ko)
{
    int chanIndex = 0;
    if(_isGroup)
    {
        chanIndex = (ko.asap() - GRP_KoOffset) % GRP_KoBlockSize;
        logDebugP("Got GROUP KO %i", chanIndex);
    } else {
        chanIndex = (ko.asap() - ADR_KoOffset) % ADR_KoBlockSize;
        logDebugP("Got SHORT KO %i", chanIndex);
    }

    switch(chanIndex)
    {
        //Schalten
        case ADR_Koswitch:
            koHandleSwitch(ko);
            break;

        //Schalten Status
        //case 1

        //Dimmen relativ
        case ADR_Kodimm_relative:
            koHandleDimmRel(ko);
            break;

        //Dimmen Absolut
        case ADR_Kodimm_absolute:
            koHandleDimmAbs(ko);
            break;

        //Dimmen Status
        //case 4

        //Sperren
        case ADR_Kolock:
            koHandleLock(ko);
            break;
    
        //Error
        //case 6

        case ADR_Kocolor:
            koHandleColor(ko);
            break;
    }
}

void DaliChannel::koHandleSwitch(GroupObject &ko)
{
    logDebugP("Schalten");
    if(isLocked)
    {
        logErrorP("is locked");
        return;
    }

    if(_isStaircase)
    {
        if(ko.value(DPT_Switch))
        {
            
            if(state)
            {
                logDebugP("ist bereits an");
                if(ParamADR_nachtriggern)
                {
                    logDebugP("wurde nachgetriggert");
                    startTime = millis();
                    return;
                }
                return;
            }
            logDebugP(isNight ? "Einschalten Nacht" : "Einschalten Tag");
            state = true;
            startTime = millis();
            logDebugP("interval %i", interval);

            uint8_t onValue = isNight ? _onNight : _onDay;
            if(onValue == 0) onValue = isNight ? _lastNightValue : _lastDayValue;
            sendArc(onValue);
            setDimmState(DaliHelper::percentToArc(onValue));
        }
        else
        {
            bool manuOff = ParamADR_manuoff;
            if(!manuOff)
            {
                logErrorP("no manuel off");
                return;
            }

            logDebugP("Ausschalten");
            sendArc(0x00);
            setSwitchState(false);
            state = false;
        }
    } else {
        bool value = ko.value(DPT_Switch);
        if(value)
        {
            uint8_t onValue = isNight ? _onNight : _onDay;
            if(onValue == 0) onValue = isNight ? _lastNightValue : _lastDayValue;
            logDebugP(isNight ? "Einschalten Nacht" : "Einschalten Tag");
            sendArc(onValue);
            setDimmState(DaliHelper::percentToArc(onValue));
        } else {
            logDebugP("Ausschalten");
            sendArc(0x00);
            setSwitchState(false);
        }
    }
}

void DaliChannel::koHandleDimmRel(GroupObject &ko)
{
    logDebugP("Dimmen relativ");
    if(isLocked)
    {
        logErrorP("is locked");
        return;
    }

    _dimmStep = ko.value(Dpt(3,7,1));

    if(_dimmStep == 0)
    {
        _dimmDirection = DimmDirection::None;
        _dimmLast = 0;
        setDimmState(_lastStep, true, true);
        logDebugP("Dimm Stop");
        return;
    }

    uint8_t toSet = _lastStep;
    _dimmDirection = ko.value(Dpt(3,7,0)) ? DimmDirection::Up : DimmDirection::Down;
    if(_dimmDirection == DimmDirection::Up)
    {
        logDebugP("Dimm Up Start %i", toSet);
    } else if(_dimmDirection == DimmDirection::Down) {
        logDebugP("Dimm Down Start %i", toSet);
    }
}

void DaliChannel::koHandleDimmAbs(GroupObject &ko)
{
    logDebugP("Dimmen absolut");
    if(isLocked)
    {
        logErrorP("is locked");
        return;
    }

    uint8_t value = ko.value(Dpt(5,1));
    logDebugP("Dimmen Absolut auf %i%%", value);
    sendArc(value);
    setDimmState(DaliHelper::percentToArc(value), true, true);
}

void DaliChannel::koHandleLock(GroupObject & ko)
{    
    bool value = ko.value(Dpt(1,1));

    if(_isGroup)
    {
        if(ParamGRP_locknegate)
            value = !value;
    } else {
        if(ParamADR_locknegate)
            value = !value;
    }

    if(isLocked == value) return;
    isLocked = value;
    uint8_t behave;
    uint8_t behavevalue;
    if(isLocked)
    {
        logDebugP("Sperren");
        behave = ParamADR_lockbehave;
        behavevalue = ParamADR_lockvalue;
    } else {
        logDebugP("Entsperren");
        behave = ParamADR_unlockbehave;
        behavevalue = ParamADR_unlockvalue;
    }

    switch(behave)
    {
        //nothing
        case 0:
            logDebugP("Nichts");
            return;

        //Ausschalten
        case 1:
        {
            logDebugP("Ein");
            behavevalue = isNight ? _onNight : _onDay;
            break;
        }

        //Einschalten
        case 2:
        {
            logDebugP("Aus");
            behavevalue = 0;
            break;
        }

        //Fester Wert
        case 3:
        {
            logDebugP("Wert");
            break;
        }
    }
    logDebugP("%i - %i", behavevalue, DaliHelper::percentToArc(behavevalue));
    sendArc(behavevalue);
    setDimmState(DaliHelper::percentToArc(behavevalue));
}

void DaliChannel::koHandleColor(GroupObject &ko)
{
    bool isRGB = ParamADR_colorType;
    uint32_t value = ko.value(Dpt(232,600));
    logDebugP("Got Color: %X", value);
    uint8_t r, g, b;

    if(!isRGB)
    {
        //TODO HSV to RGB
        ColorHelper::hsvToRGB((uint8_t)(value >> 16), (uint8_t)((value >> 8) & 0xFF), (uint8_t)(value & 0xFF), r, g, b);
    } else {
        r = value >> 16;
        g = (value >> 8) & 0xFF;
        b = value & 0xFF;
    }

    logDebugP("RGB: %i %i %i", r, g, b);
    if(r == 255) r--;
    if(g == 255) g--;
    if(b == 255) b--;
    logDebugP("Send: %i %i %i", r, g, b);
    
    uint8_t sendType = ParamADR_colorSpace;
    switch(sendType)
    {
        //send as rgb
        case 0:
        {
            sendSpecialCmd(DaliSpecialCmd::SET_DTR, r);
            sendSpecialCmd(DaliSpecialCmd::SET_DTR1, g);
            sendSpecialCmd(DaliSpecialCmd::SET_DTR2, b);
            sendSpecialCmd(DaliSpecialCmd::ENABLE_DT, 8);
            sendCmd(DaliCmd::SET_TEMP_RGB);
            sendSpecialCmd(DaliSpecialCmd::ENABLE_DT, 8);
            sendCmd(DaliCmd::ACTIVATE);
            break;
        }

        //send as xy
        case 1:
        {
            uint16_t x;
            uint16_t y;
            ColorHelper::rgbToXY(r, g, b, x, y);

            sendSpecialCmd(DaliSpecialCmd::SET_DTR, x & 0xFF);
            sendSpecialCmd(DaliSpecialCmd::SET_DTR, (x >> 8) & 0xFF);
            sendSpecialCmd(DaliSpecialCmd::ENABLE_DT, 8);
            sendCmd(DaliCmd::SET_COORDINATE_X);
            
            sendSpecialCmd(DaliSpecialCmd::SET_DTR, y & 0xFF);
            sendSpecialCmd(DaliSpecialCmd::SET_DTR1, (y >> 8) & 0xFF);
            sendSpecialCmd(DaliSpecialCmd::ENABLE_DT, 8);
            sendCmd(DaliCmd::SET_COORDINATE_Y);
            
            sendSpecialCmd(DaliSpecialCmd::ENABLE_DT, 8);
            sendCmd(DaliCmd::ACTIVATE);
            break;
        }
    }

    setDimmState(254, true, true); //TODO get real 
}

void DaliChannel::setSwitchState(bool value, bool isSwitchCommand)
{
    if(isSwitchCommand)
    {
        uint8_t toSet = 0;
        if(value)
        {
            toSet = DaliHelper::percentToArc(isNight ? _onNight : _onDay);
        } else {
            toSet = 0;
        }

        setDimmState(toSet);
        _lastStep = toSet;
    }

    if(value == _lastState) return;
    _lastState = value;

    if(_isGroup)
        knx.getGroupObject(calcKoNumber(GRP_Koswitch_state)).value(value, DPT_Switch);
    else
        knx.getGroupObject(calcKoNumber(ADR_Koswitch_state)).value(value, DPT_Switch);
}

void DaliChannel::setDimmState(uint8_t value, bool isDimmCommand, bool isLastCommand)
{
    if(_dimmDirection == DimmDirection::None && value > 0)
    {
        if(isNight)
            _lastNightValue = value;
        else
            _lastDayValue = value;
    }

    if(isDimmCommand)
    {
        setSwitchState(value > 0, false);
        _lastStep = value;
    }

    //TODO arcToPercent seems to not work!
    uint8_t perc = DaliHelper::arcToPercent(value);
    logDebugP("SetDimmState %i/%i (%i)", perc, value, _lastValue);

    if(perc == _lastValue) return;
    _lastValue = perc;

    if(_isGroup)
        knx.getGroupObject(calcKoNumber(GRP_Kodimm_state)).value(perc, Dpt(5, 1));
    else
        knx.getGroupObject(calcKoNumber(ADR_Kodimm_state)).value(perc, Dpt(5, 1));
}

void DaliChannel::setOnValue(uint8_t value)
{
    _onDay = value;
}

void DaliChannel::setGroups(uint16_t groups)
{
    _groups = groups;
}

void DaliChannel::setGroupState(uint8_t group, bool state)
{
    if(_groups & 1 << group)
        setSwitchState(state, false);
}

void DaliChannel::setGroupState(uint8_t group, uint8_t value)
{
    if(_groups & (1 << group))
        setDimmState(DaliHelper::percentToArc(value), true, true);
}

void DaliChannel::setMinMax(uint8_t min, uint8_t max)
{
    _min = min;
    _max = max;
}

uint8_t DaliChannel::getMin()
{
    return _min;
}

uint8_t DaliChannel::getMax()
{
    return _max;
}

uint16_t DaliChannel::getGroups()
{
    return _groups;
}
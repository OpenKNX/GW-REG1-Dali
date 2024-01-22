#include "DaliChannel.h"
#include "OpenKNX.h"

DaliChannel::DaliChannel(MessageQueue &queue) : _queue(queue) {}

DaliChannel::~DaliChannel() {}

const std::string DaliChannel::name()
{
    if (_isStaircase)
        if (_isGroup)
            return "StaircaseChannel_G";
        else
            return "StaircaseChannel_A";

    if (_isGroup)
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

bool DaliChannel::hasError()
{
    return _errorState;
}

void DaliChannel::init(uint8_t channelIndex, bool ig)
{
    _channelIndex = channelIndex;
    _isGroup = ig;
}

// will be called once
// only if knx.configured == true
void DaliChannel::setup()
{
    if (_isGroup)
    {
        _isConfigured = ParamGRP_deviceType != 0;
        if (!_isConfigured)
        {
            logDebugP("Nicht konfiguriert");
            return;
        }

        _isStaircase = ParamGRP_type;
        _min = 0;
        _max = 0xFF;
        if (_isStaircase)
            interval = ParamGRP_stairtime;
        _onDay = ParamGRP_onDay;
        _onNight = ParamGRP_onNight;
    }
    else
    {
        _isConfigured = ParamADR_deviceType != 15;
        if (!_isConfigured)
        {
            logDebugP("Nicht konfiguriert");
            return;
        }

        _isStaircase = ParamADR_type;
        _min = ParamADR_min;
        _max = ParamADR_max;
        if (_isStaircase)
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
    if (!_isConfigured)
        return;

    loopStaircase();
    loopDimming();
    loopError();
}

void DaliChannel::loopStaircase()
{
    if (_isStaircase && currentState)
    {
        if (millis() - startTime > interval * 1000)
        {
            logDebugP("Zeit abgelaufen");
            currentState = false;
            sendArc(0x00);
            setSwitchState(false);
        }
    }
}

void DaliChannel::loopDimming()
{
    if (_dimmDirection != DimmDirection::None && millis() - _dimmLast > DimmInterval)
    {
        if (_dimmDirection == DimmDirection::Up)
        {
            if (currentDimmType == DimmType::Brigthness)
                sendCmd(DaliCmd::ON_AND_STEP_UP);
            *currentDimmValue++;
        }
        else if (_dimmDirection == DimmDirection::Down)
        {
            if (currentDimmType == DimmType::Brigthness)
                sendCmd(DaliCmd::STEP_DOWN_AND_OFF);
            *currentDimmValue--;
        }

        if (*currentDimmValue == 0)
        {
            logDebugP("Dimm Stop at 0");
            _dimmDirection = DimmDirection::None;
        }
        else if (*currentDimmValue == 254)
        {
            logDebugP("Dimm Stop at 254");
            _dimmDirection = DimmDirection::None;
        }

        updateCurrentDimmValue(false);
        _dimmLast = millis();
    }
}

void DaliChannel::loopError()
{
    if (!_isGroup && _getError)
    {
        if (_errorResp != 300)
        {
            int8_t resp = _queue.getResponse(_errorResp);
            if (resp == -200)
                return;
            _errorResp = 300;
            if (resp == -1)
                resp = 1;
            if (resp < 0)
                return;

            resp = resp & 0b11;
            bool val = knx.getGroupObject(calcKoNumber(ADR_Koerror)).value(Dpt(1, 0));
            _errorState = val != 0;
            if (val != resp)
                knx.getGroupObject(calcKoNumber(ADR_Koerror)).value((val != 0), DPT_Switch);
        }

        if (millis() - _lastError > 60000)
        {
            _errorResp = sendCmd(144); // QUERY_STATUS
            _lastError = millis();
        }
    }
}

uint16_t DaliChannel::calcKoNumber(int asap)
{
    if (_isGroup)
        return asap + (GRP_KoBlockSize * _channelIndex) + GRP_KoOffset;

    return asap + (ADR_KoBlockSize * _channelIndex) + ADR_KoOffset;
}

uint8_t DaliChannel::sendArc(byte v)
{
    Message *msg = new Message();
    msg->id = _queue.getNextId();
    msg->type = MessageType::Arc;
    msg->para1 = _channelIndex;
    msg->para2 = DaliHelper::percentToArc(v);
    msg->addrtype = _isGroup;
    return _queue.push(msg);
}

uint8_t DaliChannel::sendCmd(byte cmd)
{
    Message *msg = new Message();
    msg->id = _queue.getNextId();
    msg->type = MessageType::Cmd;
    msg->para1 = _channelIndex;
    msg->para2 = cmd;
    msg->addrtype = _isGroup;
    return _queue.push(msg);
}

uint8_t DaliChannel::sendSpecialCmd(DaliSpecialCmd cmd, byte value)
{
    Message *msg = new Message();
    msg->id = _queue.getNextId();
    msg->type = MessageType::SpecialCmd;
    msg->para1 = static_cast<uint8_t>(cmd);
    msg->para2 = value;
    msg->addrtype = _isGroup;
    return _queue.push(msg);
}

void DaliChannel::processInputKo(GroupObject &ko)
{
    int chanIndex = 0;
    if (_isGroup)
    {
        chanIndex = (ko.asap() - GRP_KoOffset) % GRP_KoBlockSize;
        logDebugP("Got GROUP KO %i", chanIndex);
    }
    else
    {
        chanIndex = (ko.asap() - ADR_KoOffset) % ADR_KoBlockSize;
        logDebugP("Got SHORT KO %i", chanIndex);
    }

    switch (chanIndex)
    {
    // Schalten
    case ADR_Koswitch:
        koHandleSwitch(ko);
        break;

    // Schalten Status
    // case 1

    // Dimmen relativ
    case ADR_Kodimm_relative:
        koHandleDimmRel(ko);
        break;

    // Dimmen Absolut
    case ADR_Kodimm_absolute:
        koHandleDimmAbs(ko);
        break;

    // Dimmen Status
    // case 4

    // Sperren
    case ADR_Kolock:
        koHandleLock(ko);
        break;

        // Error
        // case 6

    case ADR_Kocolor:
        koHandleColor(ko);
        break;

    // Farbe Status
    // case 8

    // Farbe Rot Dimmen relativ
    case ADR_Kocolor_red_relative:
        koHandleColorRel(ko, 0);
        break;

    // Farbe Rot Dimmen absolut
    case ADR_Kocolor_red_absolute:
        koHandleColorAbs(ko, 0);
        break;

    // Farbe Rot Status
    // case 11

    // Farbe Grün Dimmen relativ
    case ADR_Kocolor_green_relative:
        koHandleColorRel(ko, 1);
        break;

    // Farbe Grün Dimmen absolut
    case ADR_Kocolor_green_absolute:
        koHandleColorAbs(ko, 1);
        break;

    // Farbe Grün Status
    // case 14

    // Farbe Blau Dimmen relativ
    case ADR_Kocolor_blue_relative:
        koHandleColorRel(ko, 2);
        break;

    // Farbe Blau Dimmen absolut
    case ADR_Kocolor_blue_absolute:
        koHandleColorAbs(ko, 2);
        break;

        // Farbe Blau Status
        // case 17
    }
}

void DaliChannel::koHandleColorRel(GroupObject &ko, uint8_t index)
{
    logDebugP("Farbe absolut %i", index);
    if (currentIsLocked)
    {
        logErrorP("is locked");
        return;
    }

    _dimmStep = ko.value(Dpt(3, 7, 1));

    if (_dimmStep == 0)
    {
        _dimmDirection = DimmDirection::None;
        _dimmLast = 0;
        updateCurrentDimmValue(true);
        logDebugP("Dimm Stop");
        return;
    }

    currentDimmValue = &currentStep;
    currentDimmType = DimmType::Color;
    _dimmDirection = ko.value(Dpt(3, 7, 0)) ? DimmDirection::Up : DimmDirection::Down;
    if (_dimmDirection == DimmDirection::Up)
    {
        logDebugP("Dimm Up Start %i", currentStep);
    }
    else if (_dimmDirection == DimmDirection::Down)
    {
        logDebugP("Dimm Down Start %i", currentStep);
    }
}

void DaliChannel::koHandleColorAbs(GroupObject &ko, uint8_t index)
{
    logDebugP("Farbe absolut %i", index);
    if (currentIsLocked)
    {
        logErrorP("is locked");
        return;
    }

    currentColor[index] = ko.value(Dpt(5, 4));
    currentDimmType = DimmType::Color;
    updateCurrentDimmValue(true);
    sendColor(true);
}

void DaliChannel::koHandleSwitch(GroupObject &ko)
{
    logDebugP("Schalten");
    if (currentIsLocked)
    {
        logErrorP("is locked");
        return;
    }

    if (_isStaircase)
        handleSwitchStaircase(ko);
    else
        handleSwitchNormal(ko);
}

void DaliChannel::handleSwitchNormal(GroupObject &ko)
{
    bool value = ko.value(DPT_Switch);
    if (value)
    {
        uint8_t onValue = isNight ? _onNight : _onDay;
        if (onValue == 0)
            onValue = isNight ? _lastNightValue : _lastDayValue;
        logDebugP(isNight ? "Einschalten Nacht" : "Einschalten Tag");
        sendArc(onValue);
        setDimmState(DaliHelper::percentToArc(onValue));
    }
    else
    {
        logDebugP("Ausschalten");
        sendArc(0x00);
        setSwitchState(false);
    }
    currentState = value;
}

void DaliChannel::handleSwitchStaircase(GroupObject &ko)
{
    if (ko.value(DPT_Switch))
    {

        if (currentState)
        {
            logDebugP("ist bereits an");

            bool retrigger = _isGroup ? ParamGRP_manuoff : ParamADR_manuoff;
            if (retrigger)
            {
                logDebugP("wurde nachgetriggert");
                startTime = millis();
                return;
            }
            return;
        }
        logDebugP(isNight ? "Einschalten Nacht" : "Einschalten Tag");
        currentState = true;
        startTime = millis();
        logDebugP("interval %i", interval);

        uint8_t onValue = isNight ? _onNight : _onDay;
        if (onValue == 0)
            onValue = isNight ? _lastNightValue : _lastDayValue;
        sendArc(onValue);
        setDimmState(DaliHelper::percentToArc(onValue));
    }
    else
    {
        bool manuOff = _isGroup ? ParamGRP_manuoff : ParamADR_manuoff;
        if (!manuOff)
        {
            logErrorP("no manuel off");
            return;
        }

        logDebugP("Ausschalten");
        sendArc(0x00);
        setSwitchState(false);
        currentState = false;
    }
}

void DaliChannel::koHandleDimmRel(GroupObject &ko)
{
    logDebugP("Dimmen relativ");
    if (currentIsLocked)
    {
        logErrorP("is locked");
        return;
    }

    _dimmStep = ko.value(Dpt(3, 7, 1));

    if (_dimmStep == 0)
    {
        _dimmDirection = DimmDirection::None;
        _dimmLast = 0;
        updateCurrentDimmValue(true);
        logDebugP("Dimm Stop");
        return;
    }

    currentDimmValue = &currentStep;
    currentDimmType = DimmType::Brigthness;
    _dimmDirection = ko.value(Dpt(3, 7, 0)) ? DimmDirection::Up : DimmDirection::Down;
    if (_dimmDirection == DimmDirection::Up)
    {
        logDebugP("Dimm Up Start %i", currentStep);
    }
    else if (_dimmDirection == DimmDirection::Down)
    {
        logDebugP("Dimm Down Start %i", currentStep);
    }
}

void DaliChannel::koHandleDimmAbs(GroupObject &ko)
{
    logDebugP("Dimmen absolut");
    if (currentIsLocked)
    {
        logErrorP("is locked");
        return;
    }

    uint8_t value = ko.value(Dpt(5, 1));
    logDebugP("Dimmen Absolut auf %i%%", value);
    sendArc(value);
    setDimmState(DaliHelper::percentToArc(value), true, true);
}

void DaliChannel::koHandleLock(GroupObject &ko)
{
    bool value = ko.value(Dpt(1, 1));

    if (_isGroup)
    {
        if (ParamGRP_locknegate)
            value = !value;
    }
    else
    {
        if (ParamADR_locknegate)
            value = !value;
    }

    if (currentIsLocked == value)
        return;
    currentIsLocked = value;
    uint8_t behave;
    uint8_t behavevalue;
    if (currentIsLocked)
    {
        logDebugP("Sperren");
        behave = ParamADR_lockbehave;
        behavevalue = ParamADR_lockvalue;
    }
    else
    {
        logDebugP("Entsperren");
        behave = ParamADR_unlockbehave;
        behavevalue = ParamADR_unlockvalue;
    }

    switch (behave)
    {
    // nothing
    case 0:
        logDebugP("Nichts");
        return;

    // Ausschalten
    case 1:
    {
        logDebugP("Ein");
        behavevalue = isNight ? _onNight : _onDay;
        break;
    }

    // Einschalten
    case 2:
    {
        logDebugP("Aus");
        behavevalue = 0;
        break;
    }

    // Fester Wert
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
    logDebugP("Farbe absolut");
    if (currentIsLocked)
    {
        logErrorP("is locked");
        return;
    }

    uint8_t colorType = _isGroup ? ParamGRP_colorType : ParamADR_colorType;

    // if rgb or hsv
    if (colorType < 2)
    {
        uint32_t value = ko.value(Dpt(232, 600));
        logDebugP("Got Color: %X", value);

        currentColor[0] = (value >> 16) & 0xFF;
        currentColor[1] = (value >> 8) & 0xFF;
        currentColor[2] = value & 0xFF;

        logInfoP("R=%i G=%i B=%i", currentColor[0], currentColor[1], currentColor[2]);

        // TODO send only if changed
        knx.getGroupObject(calcKoNumber(_isGroup ? 0 : ADR_Kocolor_rgb_state)).value(value, Dpt(232, 600));         // TODO fix GRP_Kocolor_rgb_state
        knx.getGroupObject(calcKoNumber(_isGroup ? 0 : ADR_Kocolor_red_state)).value(currentColor[0], Dpt(5, 4));   // TODO fix GRP_Kocolor_
        knx.getGroupObject(calcKoNumber(_isGroup ? 0 : ADR_Kocolor_green_state)).value(currentColor[1], Dpt(5, 4)); // TODO fix GRP_Kocolor_
        knx.getGroupObject(calcKoNumber(_isGroup ? 0 : ADR_Kocolor_blue_state)).value(currentColor[2], Dpt(5, 4));  // TODO fix GRP_Kocolor_

        sendColor(true);
        // if tunable white
    }
    else
    {
        uint16_t kelvin = ko.value(Dpt(7, 600));
        logDebugP("Got Kelvin: %X K", kelvin);
        uint16_t mirek = 1000000.0 / kelvin;
        sendSpecialCmd(DaliSpecialCmd::SET_DTR, mirek & 0xFF);
        sendSpecialCmd(DaliSpecialCmd::SET_DTR1, (mirek >> 8) & 0xFF);
        sendSpecialCmd(DaliSpecialCmd::ENABLE_DT, 8);
        sendCmd(DaliCmdExtendedDT8::SET_TEMPERATURE_COLOUR);

        sendSpecialCmd(DaliSpecialCmd::ENABLE_DT, 8);
        sendCmd(DaliCmd::ACTIVATE);
    }

    setDimmState(254, true, true); // TODO get real
}

void DaliChannel::sendColor(bool isLastCommand)
{
    // TODO senden nur alle 100? ms
    uint8_t r, g, b;
    uint8_t colorType = _isGroup ? ParamGRP_colorType : ParamADR_colorType;
    if (colorType == 0)
    {
        ColorHelper::hsvToRGB(currentColor[0], currentColor[1], currentColor[2], r, g, b);
    }
    else
    {
        r = currentColor[0];
        g = currentColor[1];
        b = currentColor[2];
    }

    logDebugP("RGB: %i %i %i", r, g, b);
    if (r == 255)
        r--;
    if (g == 255)
        g--;
    if (b == 255)
        b--;
    logDebugP("Send: %i %i %i", r, g, b);

    uint8_t sendType = _isGroup ? ParamGRP_colorSpace : ParamADR_colorSpace;
    switch (sendType)
    {
    // send as rgb
    case 0:
    {
        sendSpecialCmd(DaliSpecialCmd::SET_DTR, r);
        sendSpecialCmd(DaliSpecialCmd::SET_DTR1, g);
        sendSpecialCmd(DaliSpecialCmd::SET_DTR2, b);
        sendSpecialCmd(DaliSpecialCmd::ENABLE_DT, 8);
        sendCmd(DaliCmdExtendedDT8::SET_TEMP_RGB);
        sendSpecialCmd(DaliSpecialCmd::ENABLE_DT, 8);
        sendCmd(DaliCmd::ACTIVATE);
        break;
    }

    // send as xy
    case 1:
    {
        uint16_t x;
        uint16_t y;
        ColorHelper::rgbToXY(r, g, b, x, y);

        sendSpecialCmd(DaliSpecialCmd::SET_DTR, x & 0xFF);
        sendSpecialCmd(DaliSpecialCmd::SET_DTR1, (x >> 8) & 0xFF);
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
}

void DaliChannel::setSwitchState(bool value, bool isSwitchCommand)
{
    if (isSwitchCommand)
    {
        uint8_t toSet = 0;
        if (value)
        {
            toSet = DaliHelper::percentToArc(isNight ? _onNight : _onDay);
        }
        else
        {
            toSet = 0;
        }

        setDimmState(toSet);
        currentStep = toSet;
    }

    bool currentState = knx.getGroupObject(calcKoNumber(_isGroup ? GRP_Koswitch_state : ADR_Koswitch_state)).value(DPT_Switch);
    if (value == currentState)
        return;
    knx.getGroupObject(calcKoNumber(_isGroup ? GRP_Koswitch_state : ADR_Koswitch_state)).value(value, DPT_Switch);
}

void DaliChannel::setDimmState(uint8_t value, bool isDimmCommand, bool isLastCommand)
{
    if (_dimmDirection == DimmDirection::None && value > 0)
    {
        if (isNight)
            _lastNightValue = value;
        else
            _lastDayValue = value;
    }

    if (isDimmCommand)
    {
        setSwitchState(value > 0, false);
        currentStep = value;
    }

    uint8_t perc = DaliHelper::arcToPercent(value);

    uint8_t currentState = knx.getGroupObject(calcKoNumber(_isGroup ? GRP_Kodimm_state : ADR_Kodimm_state)).value(Dpt(5, 1));
    logDebugP("SetDimmState %i/%i (%i)", perc, value, currentState);
    if (perc == currentState)
        return;
    knx.getGroupObject(calcKoNumber(_isGroup ? GRP_Kodimm_state : ADR_Kodimm_state)).value(perc, Dpt(5, 1));
}

void DaliChannel::updateCurrentDimmValue(bool isLastCommand)
{
    if (currentDimmType == DimmType::Color)
        sendColor(isLastCommand);

    if (isLastCommand || millis() - lastCurrentDimmUpdate > 1000)
    {
        switch (currentDimmType)
        {
        case DimmType::Brigthness:
        {
            setDimmState(*currentDimmValue, true, false);
            break;
        }

        case DimmType::Color:
        {
            uint32_t value = currentColor[2];
            value |= currentColor[1] << 8;
            value |= currentColor[0] << 16;
            knx.getGroupObject(calcKoNumber(_isGroup ? 0 : ADR_Kocolor_rgb_state)).value(value, Dpt(232, 600));         // TODO fix GRP_Kocolor_rgb_state
            knx.getGroupObject(calcKoNumber(_isGroup ? 0 : ADR_Kocolor_red_state)).value(currentColor[0], Dpt(5, 1));   // TODO fix GRP_Kocolor_
            knx.getGroupObject(calcKoNumber(_isGroup ? 0 : ADR_Kocolor_green_state)).value(currentColor[1], Dpt(5, 1)); // TODO fix GRP_Kocolor_
            knx.getGroupObject(calcKoNumber(_isGroup ? 0 : ADR_Kocolor_blue_state)).value(currentColor[2], Dpt(5, 1));  // TODO fix GRP_Kocolor_
            break;
        }
        }
        lastCurrentDimmUpdate = millis();
    }
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
    if (_groups & 1 << group)
        setSwitchState(state, false);
}

void DaliChannel::setGroupState(uint8_t group, uint8_t value)
{
    if (_groups & (1 << group))
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
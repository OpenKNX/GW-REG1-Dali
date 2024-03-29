#include "DaliModule.h"

unsigned long daliActivity = 0;

const std::string DaliModule::name()
{
    return "Dali";
}

//You can also give it a version
//will be displayed in Command Infos 
const std::string DaliModule::version()
{
    return "";
}

void DaliModule::setCallback(EventHandlerReceivedDataFuncPtr callback)
{
    dali->setCallback(callback);
}

//will be called once
//only if knx.configured == true
void DaliModule::setup(bool conf)
{
    if(!conf) return;

    for(int i = 0; i < 64; i++)
    {
        channels[i].init(i, false);
        channels[i].setup();
    }

    
    for(int i = 0; i < 16; i++)
    {
        groups[i].init(i, true);
        groups[i].setup();
    }

    if(!conf) return;

#ifdef FUNC1_BUTTON_PIN
    openknx.func1Button.onShortClick([=] { 
        logDebugP("Func Button pressed short");
        uint8_t sett = ParamAPP_funcBtn;
        handleFunc(sett);
    });
    openknx.func1Button.onLongClick([=] { 
        logDebugP("Func Button pressed long");
        uint8_t sett = ParamAPP_funcBtnLong;
        handleFunc(sett);
    });
    openknx.func1Button.onDoubleClick([=] {
        logDebugP("Func Button pressed double");
        uint8_t sett = ParamAPP_funcBtnDbl;
        handleFunc(sett);
    });
#endif
}

#ifdef FUNC1_BUTTON_PIN
void DaliModule::handleFunc(uint8_t setting)
{
    switch(setting)
    {
        case PT_clickAction_on:
            logDebugP("Broadcast on");
            sendCmd(0xFF, DaliCmd::RECALL_MAX, 1);
            _currentIdentifyDevice = 0;
            openknx.info1Led.errorCode();
            break;
        case PT_clickAction_off:
            logDebugP("Broadcast off");
            sendCmd(0xFF, DaliCmd::OFF, 1);
            _currentIdentifyDevice = 0;
            openknx.info1Led.errorCode();
            break;
        case PT_clickAction_toggle:
            _currentToggleState = !_currentToggleState;
            logDebugP("Broadcast toggle %i", _currentToggleState);
            sendCmd(0xFF, _currentToggleState ? DaliCmd::RECALL_MAX : DaliCmd::OFF, 1);
            _currentIdentifyDevice = 0;
            openknx.info1Led.errorCode();
            break;
        case PT_clickAction_lock:
            logDebugP("Locking Device");
            _currentLockState = true;
            _currentIdentifyDevice = 0;
            openknx.info1Led.errorCode();
            break;
        case PT_clickAction_unlock:
            logDebugP("Unlocking Device");
            _currentLockState = false;
            _currentIdentifyDevice = 0;
            openknx.info1Led.errorCode();
            break;
        case PT_clickAction_lock_toggle:
            _currentLockState = !_currentLockState;
            logDebugP("Toggle Lock Device %i", _currentLockState);
            _currentIdentifyDevice = 0;
            openknx.info1Led.errorCode();
            break;
        case PT_clickAction_identify:
            _currentToggleState = true;
            openknx.info1Led.errorCode(_currentIdentifyDevice + 1);
            logDebugP("Identify Device %i", _currentIdentifyDevice);
            sendCmd(0xFF, DaliCmd::OFF, 0xFF);
            sendCmd(_currentIdentifyDevice, DaliCmd::RECALL_MAX);
            _currentIdentifyDevice++;
            if(_currentIdentifyDevice > 63)
                _currentIdentifyDevice = 0;
            break;
    }
}
#endif

#ifdef DALI_NO_TIMER
bool __isr __time_critical_func(daliTimerInterruptCallback)(repeating_timer *t)
{
    DaliBus.timerISR();
    return true;
}
#endif

daliReturnValue _lastDaliError = DALI_NO_ERROR;

void DaliModule::setup1(bool conf)
{
    dali = new DaliClass();
	dali->begin(DALI_TX, DALI_RX);
    #ifdef DALI_NO_TIMER
    alarm_pool_t *_alarmPool1 = alarm_pool_create(2, 16);
    alarm_pool_add_repeating_timer_us(_alarmPool1, -417, daliTimerInterruptCallback, NULL, &_timer);
    #endif
    DaliBus.errorCallback = [](daliReturnValue errorCode) {
        _lastDaliError = errorCode;
    };
    dali->setActivityCallback([] {
        daliActivity = millis();
    });
}

void DaliModule::loop()
{
    if(_adrState != AddressingState::None) return;

    if(!_gotInitData)
    {
        if(millis() > 1000)
            loopInitData();
        return;
    }

    for(int i = 0; i < 64; i++)
    {
        channels[i].loop();
    }
    for(int i = 0; i < 16; i++)
    {
        groups[i].loop();
    }
}

unsigned long last = 0;

void DaliModule::loop1(bool configured)
{
    if(_adrState != AddressingState::None)
    {
        loopAddressing();
        loopMessages();
        return;
    }

    loopMessages();
    loopBusState();

    if(!configured) return;

    loopGroupState();
#ifdef INFO2_LED_PIN
    loopError();
#endif

    for(int i = 0; i < 64; i++)
    {
        channels[i].loop1();
    }
    for(int i = 0; i < 16; i++)
    {
        groups[i].loop1();
    }
}

void DaliModule::loopInitData()
{
    DaliChannel channel = channels[_adrFound];
    _adrFound++;

    if(channel.isConfigured())
    {
        if(_adrFound == 0)
            sendArc(0xFF, 10, 1); //turn on all at 10%

        uint16_t groups = 0;
        int16_t resp = getInfo(channel.channelIndex(), DaliCmd::QUERY_GROUPS_0_7);
        if(resp < 0)
        {
            logErrorP("Dali Error %i: Code %i", _adrFound-1, resp);
            return;
        }
        groups = resp;

        resp = getInfo(channel.channelIndex(), DaliCmd::QUERY_GROUPS_8_15);
        if(resp < 0)
        {
            logErrorP("Dali Error %i: Code %i", _adrFound-1, resp);
            return;
        }
        groups |= resp << 8;
        channel.setGroups(groups);

        sendCmd(channel.channelIndex(), DaliCmd::OFF, channel.isGroup());
    }

    if(_adrFound > 63)
    {
        _adrFound = 0;
        _gotInitData = true;
        _daliStateLast = 1;
        logInfoP("Finished init");
    }
}

void DaliModule::loopGroupState()
{
    if(_lastChangedGroup != 255)
    {
        if(_lastChangedGroup > 15)
        {
            _lastChangedGroup -= 16;
            for(int i = 0; i < 64; i++)
                channels[i].setGroupState(_lastChangedGroup, _lastChangedValue == 1);
        } else {
            for(int i = 0; i < 64; i++)
                channels[i].setGroupState(_lastChangedGroup, _lastChangedValue);
        }

        _lastChangedGroup = 255;
    }
}

#ifdef INFO2_LED_PIN
void DaliModule::loopError()
{
    bool error = false;
    for(int i = 0; i < 64; i++)
    {
        if(channels[i].hasError())
        {
            error = true;
            break;
        }
    }
    if(error)
        openknx.info2Led.on();
    else
        openknx.info2Led.off();
}
#endif

int16_t DaliModule::getInfo(byte address, int command, uint8_t additional)
{
    _daliStateLast = millis();
    uint8_t respId = sendMsg(MessageType::Cmd, address, command | additional, 0, true);
    bool gotResponse = false;
    int16_t resp = queue.getResponse(respId);
    
    while(!gotResponse)
    {
        resp = queue.getResponse(respId);

        if(resp == -1)
        {
            //error
            gotResponse = true;
            logErrorP("Got no response from channel %i", address);
        } else if(resp >= 0)
        {
            gotResponse = true;
        } else if(millis() - _daliStateLast > 2000)
        {
            logErrorP("Got no response from channel %i (2)", address);
            gotResponse = true;
        }
    }
    return (int16_t)resp;
}

void DaliModule::loopMessages()
{
    if(_lastDaliError != DALI_NO_ERROR)
    {
        switch(_lastDaliError)
        {
            case DALI_COLLISION:
                logError("Dali", "Collision!");
                break;
            case DALI_PULLDOWN:
                logError("Dali", "Pulldown");
                break;
            case DALI_CANT_BE_HIGH:
                logError("Dali", "Cant be high");
                break;
            case DALI_INVALID_STARTBIT:
                logError("Dali", "Invalid Startbit (%i/%i)", DaliBus.tempBusLevel, DaliBus.tempDelta);
                break;
            case DALI_ERROR_TIMING:
                logError("Dali", "Error Timing");
                break;
            default:
                logError("Dali", "Unknown Error %i", _lastDaliError);
                break;
        }
        _lastDaliError = DALI_NO_ERROR;
    }

    Message *msg = queue.pop();
    if(msg == nullptr) return;

    switch(msg->type)
    {
        case MessageType::Arc:
        {
            logInfoP("sending Arc");
            int16_t resp = dali->sendArcWait(msg->para1, msg->para2, msg->addrtype);
            if(msg->wait)
                queue.setResponse(msg->id, resp);
            break;
        }

        case MessageType::Cmd:
        {
            int16_t resp = dali->sendCmdWait(msg->para1, static_cast<DaliCmd>(msg->para2), msg->addrtype);
            if(msg->wait)
                queue.setResponse(msg->id, resp);
            break;
        }

        case MessageType::SpecialCmd:
        {
            int16_t resp = dali->sendSpecialCmdWait(msg->para1, msg->para2);
            if(msg->wait)
                queue.setResponse(msg->id, resp);
            break;
        }
    }

    delete[] msg;
}

void DaliModule::loopAddressing()
{
    switch(_adrState)
    {
        case AddressingState::Randomize_Wait:
        {
            if(millis() - _adrTime > DALI_WAIT_RANDOMIZE)
            {
                _adrState = AddressingState::Search;
                logInfoP("RandomizeWait finished");
            }
            break;
        }

        case AddressingState::Search:
        {
            if(_adrNoRespCounter == 2)
            {
                _adrState = AddressingState::Finish;
                break;
            }

            byte high = _adrHigh >> 16;
            byte middle = (_adrHigh >> 8) & 0xFF;
            byte low = _adrHigh & 0xFF;
            sendCmdSpecial(DaliSpecialCmd::SEARCHADDRH, high);
            sendCmdSpecial(DaliSpecialCmd::SEARCHADDRM, middle);
            sendCmdSpecial(DaliSpecialCmd::SEARCHADDRL, low);
            _adrResp = sendCmdSpecial(DaliSpecialCmd::COMPARE, 0x00, true);
            _adrTime = millis();
            _adrState = AddressingState::SearchWait;
            break;
        }

        case AddressingState::SearchWait:
        {
            int16_t response = queue.getResponse(_adrResp);
            
            if(response == -200 || response == -1)
            {
                if(response == -1 || millis() - _adrTime > DALI_WAIT_SEARCH)
                {
                    _adrLow = _adrHigh + 1;
                    _adrHigh = _adrHighLast;
                    _adrState = AddressingState::Search;
                    _adrNoRespCounter++;
                }
            } else if(response >= 0) {
                if(_adrLow == _adrHigh)
                {
                    logInfoP("Found Ballast at %.6X", _adrLow);
                    if(_adrAssign)
                    {
                        uint8_t addr = 0;
                        while(addresses[addr] == true){
                            addr++;
                        }
                        addresses[addr] = true;
                        logInfoP("Assigning Address %i", addr);
                        sendCmdSpecial(DaliSpecialCmd::PROGRAMSHORT, (addr << 1) | 1);
                        sendCmdSpecial(DaliSpecialCmd::WITHDRAW);
                        ballasts[_adrFound].high = (_adrLow >> 16) & 0xFF;
                        ballasts[_adrFound].middle = (_adrLow >> 8) & 0xFF;
                        ballasts[_adrFound].low = _adrLow & 0xFF;
                        ballasts[_adrFound].address = response >> 1;
                        _adrFound++;

                        _adrLow = 0;
                        _adrHigh = 0xFFFFFF;
                        _adrHighLast = 0xFFFFFF;
                        _adrNoRespCounter = 1;
                        _adrState = AddressingState::Search;
                    } else {
                        _adrResp = sendCmdSpecial(DaliSpecialCmd::QUERY_SHORT, 0, true);
                        _adrTime = millis();
                        _adrState = AddressingState::Found;
                    }
                } else {
                    _adrHighLast = _adrHigh;
                    _adrHigh = (_adrLow + _adrHigh) / 2;
                    _adrNoRespCounter = 0;
                    _adrState = AddressingState::Search;
                }
            } else {
                logErrorP("Dali Error %i, aborting addressing", response);
                _adrState = AddressingState::Finish;
            }
            break;
        }

        case AddressingState::Found:
        {
            int16_t response = queue.getResponse(_adrResp);
            if(response == -200 || response == -1)
            {
                if(response == -1 || millis() - _adrTime > DALI_WAIT_SEARCH)
                {
                    logErrorP("Found ballast not answering");
                    _adrState = AddressingState::Finish;
                    ballasts[_adrFound].address = 255;
                    _adrFound++;
                }
            } else if(response >= 0) {
                if(response == 255)
                    logInfoP(" -> has no Short Address");
                else
                    logInfoP(" -> has Short Address %i", response >> 1);

                ballasts[_adrFound].high = (_adrLow >> 16) & 0xFF;
                ballasts[_adrFound].middle = (_adrLow >> 8) & 0xFF;
                ballasts[_adrFound].low = _adrLow & 0xFF;
                ballasts[_adrFound].address = response >> 1;
                _adrFound++;

                _adrLow = 0;
                _adrHigh = 0xFFFFFF;
                _adrHighLast = 0xFFFFFF;
                _adrNoRespCounter = 1;
                sendCmdSpecial(DaliSpecialCmd::WITHDRAW);
                _adrState = AddressingState::Search;
            } else {
                logErrorP("Dali Error %i, aborting addressing", response);
                _adrState = AddressingState::Finish;
            }
            break;
        }

        case AddressingState::Finish:
        {
            logErrorP("Found %i ballasts", _adrFound);
            sendCmdSpecial(DaliSpecialCmd::TERMINATE);
            _adrState = AddressingState::None;
            break;
        }

        case AddressingState::Finish_Assign:
        {
            sendCmdSpecial(DaliSpecialCmd::TERMINATE);
            _adrState = AddressingState::None;
            delete[] addresses;
            delete[] ballasts;
            break;
        }

        case AddressingState::Query_Wait:
        {
            int16_t resp = queue.getResponse(_adrResp);

            if(resp == -1 || millis() - _adrTime > DALI_WAIT_SEARCH)
            {
                _adrState = AddressingState::Withdraw_Others;
                logErrorP("Adresse wird nicht verwendet");
                return;
            }

            if(resp == -200) return;

            if(resp >= 0)
            {
                logErrorP("Adresse wird bereits verwendet");
                _assState = AssigningState::Failed_Exists;
                _adrState = AddressingState::None;
                return;
            } else {
                logErrorP("Bus Error %i", resp);
                _assState = AssigningState::Failed_Bus;
                _adrState = AddressingState::None;
                return;
            }
            break;
        }

        case AddressingState::Withdraw_Others:
        {
            sendCmdSpecial(DaliSpecialCmd::INITIALISE, 0);
            sendCmdSpecial(DaliSpecialCmd::INITIALISE, 0);
            if(_adrHigh > 0)
            {
                logInfoP("Verwerfe alle mit niedrigerer Long Address");
                _adrHigh--;
                byte high = _adrHigh >> 16;
                byte middle = (_adrHigh >> 8) & 0xFF;
                byte low = _adrHigh & 0xFF;
                sendCmdSpecial(DaliSpecialCmd::SEARCHADDRH, high);
                sendCmdSpecial(DaliSpecialCmd::SEARCHADDRM, middle);
                sendCmdSpecial(DaliSpecialCmd::SEARCHADDRL, low);
                sendCmdSpecial(DaliSpecialCmd::WITHDRAW);
                _adrHigh++;
            }
            byte high = _adrHigh >> 16;
            byte middle = (_adrHigh >> 8) & 0xFF;
            byte low = _adrHigh & 0xFF;
            sendCmdSpecial(DaliSpecialCmd::SEARCHADDRH, high);
            sendCmdSpecial(DaliSpecialCmd::SEARCHADDRM, middle);
            sendCmdSpecial(DaliSpecialCmd::SEARCHADDRL, low);
            _adrResp =  sendCmdSpecial(DaliSpecialCmd::COMPARE, 0, true);
            _adrTime = millis();
            _adrState = AddressingState::Check_Address;
            break;
        }

        case AddressingState::Check_Address:
        {
            if(millis() - _adrTime > DALI_WAIT_SEARCH)
            {
                logInfoP("Device does not answer");
                _assState = AssigningState::Failed_No_Answer;
                _adrState = AddressingState::Finish_Assign;
                return;
            }

            int16_t resp = queue.getResponse(_adrResp);

            if(resp == -200) return;

            if(resp >= 0)
            {
                logInfoP("Long Address exists");
                sendCmdSpecial(DaliSpecialCmd::PROGRAMSHORT, _adrLow);
                _adrState = AddressingState::Confirm_Address;
                _adrResp = sendCmdSpecial(DaliSpecialCmd::QUERY_SHORT, 0, true);
                _adrTime = millis();
                logInfoP("Check Short Address");
                return;
            } else if(resp == -1) {
                logInfoP("Long Address dont exists");
                _assState = AssigningState::Failed_Exists_Not;
                _adrState = AddressingState::Finish_Assign;
            } else {
                logInfoP("Bus Error %i", resp);
                _assState = AssigningState::Failed_Bus;
                _adrState = AddressingState::Finish_Assign;
            }
            break;
        }

        case AddressingState::Confirm_Address:
        {
            if(millis() - _adrTime > DALI_WAIT_SEARCH)
            {
                logInfoP("Device does not answer");
                _assState = AssigningState::Failed_No_Answer;
                _adrState = AddressingState::Finish_Assign;
                return;
            }

            int16_t resp = queue.getResponse(_adrResp);

            if(resp == -200) return;

            if(resp >= 0)
            {
                if(resp == 255 && _adrLow == 255) {
                    logInfoP("Removing short address was succesfull");
                    _assState = AssigningState::Success;
                } else if(resp == _adrLow)
                {
                    logInfoP("Setting short address was succesfull");
                    _assState = AssigningState::Success;
                } else {
                    logInfoP("Setting short address failed");
                    _assState = AssigningState::Failed_Confirm;
                }
                _adrState = AddressingState::Finish_Assign;
                return;
            } else {
                logInfoP("Bus Error %i", resp);
                _assState = AssigningState::Failed_Bus;
                _adrState = AddressingState::Finish_Assign;
                return;
            }
            break;
        }

        case AddressingState::SearchAdr:
        {
            if(_adrLow == 64)
            {
                sendCmdSpecial(DaliSpecialCmd::INITIALISE, 255);
                sendCmdSpecial(DaliSpecialCmd::INITIALISE, 255);
                //sendCmdSpecial(DaliSpecialCmd::RANDOMISE);
                //sendCmdSpecial(DaliSpecialCmd::RANDOMISE);
                _adrState = AddressingState::Randomize_Wait;
                _adrLow = 0;
                _adrHigh = 0xFFFFFF;
                _adrHighLast = 0xFFFFFF;
                _adrNoRespCounter = 0;
                break;
            }
                
            _adrState = AddressingState::SearchAdrWait;
            _assState = AssigningState::Working;
            _adrResp = sendCmd(_adrLow, DaliCmd::QUERY_STATUS, DaliAddressTypes::SHORT, true);
            _adrTime = millis();
            break;
        }
        
        case AddressingState::SearchAdrWait:
        {
            if(millis() - _adrTime > DALI_WAIT_SEARCH)
            {
                logInfoP("Address %i does not exists", _adrLow);
                _adrState = AddressingState::SearchAdr;
                _adrLow++;
                return;
            }

            int16_t resp = queue.getResponse(_adrResp);

            if(resp == -200) return;

            if(resp >= 0)
            {
                logInfoP("Address %i exists", _adrLow);
                ballasts[_adrFound].address = _adrLow;
                _adrState = AddressingState::SearchAdr;
                if(_adrAssign)
                    addresses[_adrLow] = true;
                _adrLow++;
            } else if(resp == -1) {
                logInfoP("Address %i does not exist", _adrLow);
                _adrState = AddressingState::SearchAdr;
                _adrLow++;
            } else {
                logInfoP("Bus Error %i", resp);
                _assState = AssigningState::Failed_Bus;
                _adrState = AddressingState::Finish_Assign;
            }
            break;
        }
    }
}

void DaliModule::loopBusState()
{
    bool state = !digitalRead(DALI_RX);
#ifdef INFO3_LED_PIN
    if(_lastBusState != state)
    {
        _lastBusState = state;

        if(state)
            openknx.info3Led.activity(daliActivity, true);
        else
            openknx.info3Led.off();
    }
#endif
    if(state != _daliBusStateToSet)
    {
        _daliBusStateToSet = state;
        _daliStateLast = millis();
        if(_daliStateLast == 0) _daliStateLast = 1;
    } else if(_daliStateLast != 0 && millis() - _daliStateLast > 1000)
    {
        _daliStateLast = 0;
        if(_daliBusState != _daliBusStateToSet)
        {
            _daliBusState = _daliBusStateToSet;
            if(_daliBusState)
                logInfoP("Dali Busspg. vorhanden");
            else
                logInfoP("Dali Busspg. nicht vorhanden");
        }
    }
}

bool DaliModule::getDaliBusState()
{
    return _daliBusState;
}

void DaliModule::showHelp()
{
    openknx.console.printHelpLine("scan", "Dali scan for EVGs");
    openknx.console.printHelpLine("auto", "Dali scan for unaddressed EVGs and assign free address");
    openknx.console.printHelpLine("arc", "Dali set Value for EVG, Group or Broadcast");
    openknx.console.printHelpLine("set", "Dali set EVG short address");
    openknx.console.printHelpLine("stepUp", "stepUp x => send x times StepUp");
    openknx.console.printHelpLine("stepDown", "stepDown xyy => send x times StepDown to evg y");
    openknx.console.printHelpLine("getLvl", "getLvl yy => get level of evg y");
}

bool DaliModule::processCommand(const std::string cmd, bool diagnoseKo)
{
    if(diagnoseKo) return false;

	std::size_t pos = cmd.find(' ');
	std::string command;
	std::string arg;
    bool hasArg = false;
	if(pos != -1)
	{
		command = cmd.substr(0, pos);
		arg = cmd.substr(pos+1, cmd.length() - pos - 1);
        hasArg = true;
	} else {
		command = cmd;
	}

    if(command == "scan")
    {
        cmdHandleScan(hasArg, arg);
        return true;
    } 
    if(command == "arc")
    {
        cmdHandleArc(hasArg, arg);
        return true;
    }
    if(command == "set")
    {
        cmdHandleSet(hasArg, arg);
        return true;
    }
    if(command == "auto")
    {
        cmdHandleAuto(hasArg, arg);
        return true;
    }
    if(command == "stepUp")
    {
        uint8_t value = std::stoi(arg.substr(0, 1));
        uint8_t addr = std::stoi(arg.substr(1, 2));
        for(int i = 0; i < value; i++)
            sendCmd(addr, DaliCmd::STEP_UP);
        return true;
    }
    if(command == "stepDown")
    {
        uint8_t value = std::stoi(arg.substr(0, 1));
        uint8_t addr = std::stoi(arg.substr(1, 2));
        for(int i = 0; i < value; i++)
            sendCmd(addr, DaliCmd::STEP_DOWN);
        return true;
    }

    if(command == "getLvl")
    {
        uint8_t addr = std::stoi(arg);
        int16_t resp = getInfo(addr, DaliCmd::QUERY_ACTUAL_LEVEL);
        if(resp >= 0)
            logDebugP("EVG %i has level %i = %.2f", addr, resp, DaliHelper::arcToPercentFloat((uint8_t)resp));
        else if(resp == -1)
            logDebugP("EVG %i antwortet nicht", addr);
        else
            logDebugP("Fehler beim Auslesen %i", resp);
        return true;
    }

    return false;
}

void DaliModule::cmdHandleScan(bool hasArg, std::string arg)
{
    if(!hasArg || arg.length() != 2)
    {
        logErrorP("Argument is invalid!");
        logIndentUp();
        logErrorP("scan xy");
        logErrorP("x = 0 => all EVGs");
        logErrorP("    1 => only unaddressed");
        logErrorP("y = 0 => don't randomize");
        logErrorP("    1 => do randomize");
        logIndentDown();
        return;
    }
    logInfoP("Starting Scan manually");
    uint8_t resultLength = 254;
    uint8_t* data = new uint8_t[4];
    uint8_t* resultData = new uint8_t[4];
    data[1] = arg.at(0) == '1';
    data[2] = arg.at(1) == '1';

    funcHandleScan(data, resultData, resultLength);
    delete[] data;
    delete[] resultData;
}

void DaliModule::cmdHandleArc(bool hasArg, std::string arg)
{
    if(!hasArg || arg.length() != 6)
    {
        logErrorP("Argument is invalid! %i", arg.length());
        logIndentUp();
        logErrorP("arc XYYZZZ");
        logErrorP("X = B => Broadcast");
        logErrorP("    A => Short Address");
        logErrorP("    G => Group");
        logErrorP("Y = Address (00-63)");
        logErrorP("    Group   (00-15)");
        logErrorP("    Broadc. (00)");
        logErrorP("Z = Percent Value (000-100)");
        logIndentDown();
        return;
    }

    uint8_t value = std::stoi(arg.substr(3, 3));
    if(arg.at(0) == 'B')
    {
        logInfoP("Sending Arc %i to Broadcast");
        sendArc(0xFF, value, DaliAddressTypes::GROUP);
    } else if(arg.at(0) == 'A')
    {
        uint8_t addr = std::stoi(arg.substr(1, 2));
        logInfoP("Sending Arc %i to EVG %i", value, addr);
        sendArc(addr, value, DaliAddressTypes::SHORT);
    } else if(arg.at(0) == 'G')
    {
        uint8_t addr = std::stoi(arg.substr(1, 2));
        logInfoP("Sending Arc %i to Group %i", value, addr);
        sendArc(addr, value, DaliAddressTypes::GROUP);
    } else {
        logErrorP("Argument X is invalid!");
    }
}

void DaliModule::cmdHandleSet(bool hasArg, std::string arg)
{
    if(!hasArg || arg.length() != 8)
    {
        logErrorP("Argument is invalid! %i", arg.length());
        logIndentUp();
        logErrorP("set XXXXXXYY");
        logErrorP("X = Long Address  (000000-ffffff)");
        logErrorP("Y = Short Address (00-63; 99=unadressiert)");
        logIndentDown();
        return;
    }

    uint8_t resultLength = 254;
    uint8_t* data = new uint8_t[4];
    uint8_t* resultData = new uint8_t[4];
    data[1] = std::stoi(arg.substr(6,2));
    data[2] = std::stoi(arg.substr(0,2), nullptr, 16);
    data[3] = std::stoi(arg.substr(2,2), nullptr, 16);
    data[4] = std::stoi(arg.substr(4,2), nullptr, 16);

    funcHandleAssign(data, resultData, resultLength);
    delete[] data;
    delete[] resultData;
}

void DaliModule::cmdHandleAuto(bool hasArg, std::string arg)
{
    uint8_t resultLength = 254;
    uint8_t* data = new uint8_t[4];
    uint8_t* resultData = new uint8_t[4];
    funcHandleAddress(data, resultData, resultLength);
    delete[] data;
    delete[] resultData;
}

void DaliModule::processInputKo(GroupObject &ko)
{
    logDebugP("Received Ko");
    if(_adrState != AddressingState::None || _currentLockState) return;

    int koNum = ko.asap();
    logDebugP("Got ko %i", koNum);
    if(koNum >= ADR_KoOffset && koNum < ADR_KoOffset + ADR_KoBlockSize * 64)
    {
        int index = floor((koNum - ADR_KoOffset) / ADR_KoBlockSize);
        logDebugP("For Channel %i", index);
        channels[index].processInputKo(ko);
        return;
    }

    if(koNum >= GRP_KoOffset && koNum < GRP_KoOffset + GRP_KoBlockSize * 16)
    {
        int index = floor((koNum - GRP_KoOffset) / GRP_KoBlockSize);
        int chanIndex = (ko.asap() - GRP_KoOffset) % GRP_KoBlockSize;
        logDebugP("For Group %i", index);
        groups[index].processInputKo(ko);

        if(chanIndex == GRP_Koswitch_state)
        {
            _lastChangedGroup = index + 16;
            _lastChangedValue = ko.value(Dpt(1,1));
        }
        if(chanIndex == GRP_Kodimm_state)
        {
            _lastChangedGroup = index;
            _lastChangedValue = ko.value(Dpt(5,1));
        }
        
        return;
    }

    switch(koNum)
    {
        //broadcast switch
        case APP_Kobroadcast_switch:
            koHandleSwitch(ko);
            break;

        //broadcast dimm absolute
        case APP_Kobroadcast_dimm:
            koHandleDimm(ko);
            break;

        //Tag/Nacht Objekt
        case APP_Kodaynight:
            koHandleDayNight(ko);
            break;

        //Set OnValue Day
        case APP_KoonValue:
            koHandleOnValue(ko);
            break;

        case APP_Koscene:
            koHandleScene(ko);
            break;
    }
}

void DaliModule::koHandleSwitch(GroupObject & ko)
{
    bool value = ko.value(DPT_Switch);
    logDebugP("Broadcast Switch %i", value);
    dali->sendArcBroadcast(value ? 0xFE : 0x00);
}

void DaliModule::koHandleDimm(GroupObject & ko)
{
    uint8_t value = ko.value(Dpt(5,1));
    logDebugP("Broadcast Dimm %i", value);
    value = ((253/3)*(log10(value)+1)) + 1;
    value++;
    dali->sendArcBroadcast(value);
}

void DaliModule::koHandleDayNight(GroupObject & ko)
{
    bool value = ko.value(DPT_Switch);
    if(ParamAPP_daynight) value = !value;
    logDebugP("Broadcast Day/Night %i", value);
    if(ParamAPP_daynight) value = !value;

    for(int i = 0; i < 64; i++)
        channels[i].isNight = value;
    for(int i = 0; i < 16; i++)
        groups[i].isNight = value;
}

void DaliModule::koHandleOnValue(GroupObject & ko)
{
    uint8_t value = ko.value(Dpt(5,1));
    logDebugP("KO OnValue: %i", value);

    for(int i = 0; i < 64; i++)
        channels[i].setOnValue(value);
    for(int i = 0; i < 16; i++)
        groups[i].setOnValue(value);
}

void DaliModule::koHandleScene(GroupObject & ko)
{
    uint8_t gotNumber = ko.value(DPT_SceneNumber);
    logDebugP("KO Scene: %i", gotNumber);
    for(int i = 0; i < SCE_CountNumber; i++)
    {
        uint8_t dest = ParamSCE_typeIndex(i);
        logDebugP("KO Scene%i: Dest=%i", i, dest);
        if(dest == 0) continue;
        uint8_t number = ParamSCE_numberKnxIndex(i);
        logDebugP("KO Scene%i: Number=%i", i, number-1);
        if(gotNumber == number - 1)
        {
            bool isSave = ko.value(Dpt(18,1,0));
            logDebugP("KO Scene%i: Save=%i", i, isSave);
            if(isSave && !ParamSCE_saveIndex(i))
            {
                logDebugP("KO Scene%i: Save not allowed", i);
                continue;
            }

            uint8_t scene = ParamSCE_numberDaliIndex(i);
            logDebugP("KO Scene%i: Scene=%i", i, scene);
            uint8_t addr = 0;
            uint8_t type = 0;
            switch(dest)
            {
                //Address
                case PT_scenetype_address:
                {
                    addr = ParamSCE_addressIndex(i);
                    logDebugP("KO Scene%i: Addr=%i", i, addr);
                    type = static_cast<uint8_t>(DaliAddressTypes::SHORT);
                    break;
                }

                //Group
                case PT_scenetype_group:
                {
                    addr = ParamSCE_groupIndex(i);
                    logDebugP("KO Scene%i: Grou=%i", i, addr);
                    type = static_cast<uint8_t>(DaliAddressTypes::GROUP);
                    break;
                }

                //Broadcast
                case PT_scenetype_broadcast:
                {
                    addr = 0xFF;
                    logDebugP("KO Scene%i: Broadcast", i);
                    type = static_cast<uint8_t>(DaliAddressTypes::GROUP);
                    break;
                }
            }

            if(isSave)
            {
                sendCmd(addr, DaliCmd::ARC_TO_DTR, type);
                uint8_t temp = static_cast<uint8_t>(DaliCmd::DTR_AS_SCENE);
                temp |= scene;
                sendCmd(addr, static_cast<DaliCmd>(temp), type);
            } else {
                uint8_t temp = static_cast<uint8_t>(DaliCmd::GO_TO_SCENE);
                temp |= scene;
                sendCmd(addr, static_cast<DaliCmd>(temp), type);
            }
        }
    }
}

bool DaliModule::processFunctionProperty(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t *data, uint8_t *resultData, uint8_t &resultLength)
{
    if(objectIndex != 160 || propertyId != 1) return false;

    switch(data[0])
    {
        case 2:
            funcHandleType(data, resultData, resultLength);
            return true;

        case 3:
            funcHandleScan(data, resultData, resultLength);
            return true;

        case 4:
            funcHandleAssign(data, resultData, resultLength);
            return true;
        
        case 5:
            funcHandleAddress(data, resultData, resultLength);
            return true;

        case 10:
            funcHandleEvgWrite(data, resultData, resultLength);
            return true;

        case 11:
            funcHandleEvgRead(data, resultData, resultLength);
            return true;

        case 12:
            funcHandleSetScene(data, resultData, resultLength);
            return true;

        case 13:
            funcHandleGetScene(data, resultData, resultLength);
            return true;

        case 14:
            funcHandleIdentify(data, resultData, resultLength);
            return true;
    }


    return false;
}

void DaliModule::funcHandleType(uint8_t *data, uint8_t *resultData, uint8_t &resultLength)
{
    int16_t resp = getInfo(data[1], DaliCmd::QUERY_DEVICE_TYPE);
    if(resp < 0)
    {
        logErrorP("Dali Error (DT): Code %i", resp);
        resultData[0] = 0x01;
        resultLength = 1;
        return;
    }
    logDebugP("Resp: %.2X", resp);

    uint8_t deviceType = resp;
    if(resp == 255) // means evg supports several devicetypes
    {
        while(true)
        {
            resp = getInfo(data[1], DaliCmd::QUERY_NEXT_DEVTYPE);
            if(resp < 0)
            {
                logErrorP("Dali Error (NDT): Code %i", resp);
                resultData[0] = 0x01;
                resultLength = 1;
                return;
            }
            logDebugP("Resp: %.2X", resp);
            if(resp == 254)
                break;
            else
                deviceType = resp;
        }
    }


    resultData[0] = 0x00;
    resultData[1] = deviceType;

    //DeviceType Color
    if(deviceType == PT_deviceType_DT8)
    {
        sendCmdSpecial(DaliSpecialCmd::ENABLE_DT, 8);
        resp = getInfo(data[1], DaliCmdExtendedDT8::QUERY_COLOR_TYPE_FEATURES);
        if(resp < 0)
        {
            logErrorP("Dali Error (CT): Code %i", resp);
            resultData[0] = 0x02;
            resultLength = 1;
            return;
        }
        resultData[2] = resp;
        resultLength = 3;
    } else {
        resultLength = 2;
    }
}

void DaliModule::funcHandleScan(uint8_t *data, uint8_t *resultData, uint8_t &resultLength)
{
    ballasts = new Ballast[64];
    logInfoP("Starting Scan");
    _adrState = AddressingState::Init;
    _adrLow = 0;
    _adrHigh = 0xFFFFFF;
    _adrHighLast = 0xFFFFFF;
    _adrFound = 0;
    _adrAssign = false;
    
    sendCmdSpecial(DaliSpecialCmd::INITIALISE, data[1] ? 255 : 0);
    sendCmdSpecial(DaliSpecialCmd::INITIALISE, data[1] ? 255 : 0);

    if(data[1])
        logInfoP("Unaddressed only");

    if(data[2] == 1)
    {
        logInfoP("Do Randomize");
        sendCmdSpecial(DaliSpecialCmd::RANDOMISE);
        sendCmdSpecial(DaliSpecialCmd::RANDOMISE);
        _adrState = AddressingState::Randomize_Wait;
        _adrTime = millis();
        logInfoP("RandomizeWait");
    } else {
        logInfoP("Don't Randomize");
        _adrState = AddressingState::Search;
    }


    _adrNoRespCounter = 1;
    resultLength = 0;
}

void DaliModule::funcHandleAssign(uint8_t *data, uint8_t *resultData, uint8_t &resultLength)
{
    logInfoP("Starting assigning address");
    _adrResp = sendCmd(data[1], DaliCmd::QUERY_STATUS, DaliAddressTypes::SHORT, true);
    _adrTime = millis();
    _adrState = AddressingState::Query_Wait;
    _assState = AssigningState::Working;

    _adrHigh = data[2] << 16;
    _adrHigh |= data[3] << 8;
    _adrHigh |= data[4];
    logInfoP("Long  Addr %X", _adrHigh);
    
    if(data[1] == 99)
    {
        data[1] = 255;
        logInfoP("Removing Short Addr");
        _adrState = AddressingState::Withdraw_Others;
    } else {
        logInfoP("Short Addr %i", data[1]);
    }
    _adrLow = ((data[1] << 1) | 1) & 0xFF;
    
    resultLength = 0;
}

void DaliModule::funcHandleAddress(uint8_t *data, uint8_t *resultData, uint8_t &resultLength)
{
    logInfoP("Starting addressing new auto");
    _adrLow = 0;
    _adrState = AddressingState::SearchAdr;
    _adrFound = 0;
    _adrAssign = true;
    ballasts = new Ballast[64];
    addresses = new bool[64];
    for(int i = 0; i < 64; i++) addresses[i] = false;
    resultLength = 0;
}

void DaliModule::funcHandleEvgWrite(uint8_t *data, uint8_t *resultData, uint8_t &resultLength)
{
    logInfoP("Starting setting up EVG");

    DaliChannel channel = channels[data[1]];
    channel.setMinMax(data[2], data[3]);

    sendCmdSpecial(DaliSpecialCmd::SET_DTR, (data[2] == 255) ? 255 : DaliHelper::percentToArc(data[2]));
    sendCmd(data[1], DaliCmd::DTR_AS_MIN);
    sendCmdSpecial(DaliSpecialCmd::SET_DTR, (data[3] == 255) ? 255 : DaliHelper::percentToArc(data[3]));
    sendCmd(data[1], DaliCmd::DTR_AS_MAX);
    sendCmdSpecial(DaliSpecialCmd::SET_DTR, (data[4] == 255) ? 255 : DaliHelper::percentToArc(data[4]));
    sendCmd(data[1], DaliCmd::DTR_AS_POWER_ON);
    sendCmdSpecial(DaliSpecialCmd::SET_DTR, (data[5] == 255) ? 255 : DaliHelper::percentToArc(data[5]));
    sendCmd(data[1], DaliCmd::DTR_AS_FAIL);
    sendCmdSpecial(DaliSpecialCmd::SET_DTR, (data[6] >> 4) & 0xF);
    sendCmd(data[1], DaliCmd::DTR_AS_FADE_TIME);
    sendCmdSpecial(DaliSpecialCmd::SET_DTR, data[6] & 0xF);
    sendCmd(data[1], DaliCmd::DTR_AS_FADE_RATE);

    //1byte free

    uint16_t groups = data[8];
    groups |= data[9] << 8;
    channel.setGroups(groups);

    for(int i = 0; i < 16; i++)
    {
        if((groups >> i) & 0x1)
        {
            sendMsg(MessageType::Cmd, data[1], DaliCmd::ADD_TO_GROUP | i);
        } else {
            sendMsg(MessageType::Cmd, data[1], DaliCmd::REMOVE_FROM_GROUP | i);
        }
    }

    resultLength = 0;
}

void DaliModule::funcHandleEvgRead(uint8_t *data, uint8_t *resultData, uint8_t &resultLength)
{
    logInfoP("Starting reading EVG settings");
    
    resultData[0] = 0x00;

    uint8_t errorByte = 0;
    uint16_t errorByteScene = 0;

    int16_t resp = getInfo(data[1], DaliCmd::QUERY_MIN_LEVEL);
    if(resp < 0)
    {
        logErrorP("Dali Error (MIN): Code %i", resp);
        errorByte |= 0b1;
        resp = 0xFF;
    }
    logDebugP("MIN: %.2X / %.2X", resp, DaliHelper::arcToPercent(resp));
    resultData[1] = (resp == 255) ? 255 : DaliHelper::arcToPercent(resp);

    resp = getInfo(data[1], DaliCmd::QUERY_MAX_LEVEL);
    if(resp < 0)
    {
        logErrorP("Dali Error (MAX): Code %i", resp);
        errorByte |= 0b10;
        resp = 0xFF;
    }
    logDebugP("MAX: %.2X / %.2X", resp, DaliHelper::arcToPercent(resp));
    resultData[2] = (resp == 255) ? 255 : DaliHelper::arcToPercent(resp);

    resp = getInfo(data[1], DaliCmd::QUERY_POWER_ON_LEVEL);
    if(resp < 0)
    {
        logErrorP("Dali Error (POWER): Code %i", resp);
        errorByte |= 0b100;
        resp = 0xFF;
    }
    logDebugP("POWER: %.2X / %.2X", resp, DaliHelper::arcToPercent(resp));
    resultData[3] = (resp == 255) ? 255 : DaliHelper::arcToPercent(resp);

    resp = getInfo(data[1], DaliCmd::QUERY_FAIL_LEVEL);
    if(resp < 0)
    {
        logErrorP("Dali Error (FAILURE): Code %i", resp);
        errorByte |= 0b1000;
        resp = 0xFF;
    }
    logDebugP("FAILURE: %.2X / %.2X", resp, DaliHelper::arcToPercent(resp));
    resultData[4] = (resp == 255) ? 255 : DaliHelper::arcToPercent(resp);
    
    resp = getInfo(data[1], DaliCmd::QUERY_FADE_SPEEDS);
    if(resp < 0)
    {
        logErrorP("Dali Error (FAID): Code %i", resp);
        errorByte |= 0b10000;
        resp = 0xFF;
    }
    logDebugP("FAID: %.2X", resp);
    resultData[5] = resp;
    
    //1byte free

    resp = getInfo(data[1], DaliCmd::QUERY_GROUPS_0_7);
    if(resp < 0)
    {
        logErrorP("Dali Error (GROUP1): Code %i", resp);
        errorByte |= 0b1000000;
        resp = 0;
    }
    logDebugP("GROUPS0-7: %.2X", resp);
    resultData[7] = resp;
    
    resp = getInfo(data[1], DaliCmd::QUERY_GROUPS_8_15);
    if(resp < 0)
    {
        logErrorP("Dali Error (GROUP2): Code %i", resp);
        errorByte |= 0b10000000;
        resp = 0;
    }
    logDebugP("GROUPS8-15: %.2X", resp);
    resultData[8] = resp;

    resultData[9] = errorByte;
    resultData[10] = errorByteScene & 0xFF;
    resultData[11] = (errorByteScene >> 8) & 0xFF;
    resultLength = 12;
}

void DaliModule::funcHandleSetScene(uint8_t *data, uint8_t *resultData, uint8_t &resultLength)
{
    /*
    data = [
        12,
        context.Channel,
        0, //scene number
        0, //enabled
        parseInt(device.getParameterByName("deviceType").value),
        parseInt(device.getParameterByName("colorType").value),
        0, value 0-100; 255=disabled
        0, r / tw
        0, g / tw
        0  b
    ];
    */
    uint8_t addr = data[1] & 0b1111;
    uint8_t type = data[1] >> 7;

    logDebugP("Scene %i", data[2]);
    //scene is enabled
    if(data[3])
    {
        logDebugP("Bri %i", data[6]);
        //deviceType is Color
        if(data[4] == PT_deviceType_DT8)
        {
            //colorType is TunableWhite
            if(data[5] == PT_colorType_TW)
            {
                uint16_t kelvin;
                popWord(kelvin, data + 7);
                logDebugP("Temp %i", kelvin);
                uint16_t mirek = 1000000.0 / kelvin;
                logDebugP("mirek %i", mirek);
                sendCmdSpecial(DaliSpecialCmd::SET_DTR, mirek & 0xFF);
                sendCmdSpecial(DaliSpecialCmd::SET_DTR1, (mirek >> 8) & 0xFF);
                sendCmdSpecial(DaliSpecialCmd::ENABLE_DT, 0x08);
                sendCmd(addr, DaliCmdExtendedDT8::SET_TEMP_KELVIN, type);
            } else { //it is RGB
                sendCmdSpecial(DaliSpecialCmd::SET_DTR, data[7]); 
                sendCmdSpecial(DaliSpecialCmd::SET_DTR1, data[8]);
                sendCmdSpecial(DaliSpecialCmd::SET_DTR2, data[9]);
                sendCmdSpecial(DaliSpecialCmd::ENABLE_DT, 0x08);
                sendCmd(addr, DaliCmdExtendedDT8::SET_TEMP_RGB, type);
                logDebugP("RGB %.2X%.2X%.2X", data[7], data[8], data[9]);
            }
        }

        //set dimm value
        sendCmdSpecial(DaliSpecialCmd::SET_DTR, DaliHelper::percentToArc(data[6]));
        sendMsg(MessageType::Cmd, addr, DaliCmd::DTR_AS_SCENE | data[2], type);
        //     sendMsg(MessageType::Cmd, data[1], DaliCmd::DTR_AS_SCENE | i);
    } else {
        sendMsg(MessageType::Cmd, addr, DaliCmd::REMOVE_FROM_SCENE | data[2], type);
        logDebugP("disabled");
    }

    resultLength = 0;
}

void DaliModule::funcHandleGetScene(uint8_t *data, uint8_t *resultData, uint8_t &resultLength)
{
    /*
    data = [
        13,
        context.Channel,
        0, //scene number
        parseInt(device.getParameterByName("deviceType").value),
        parseInt(device.getParameterByName("colorType").value)
    ];
    */
    logDebugP("Scene %i", data[2]);
    uint8_t value = getInfo(data[1], DaliCmd::QUERY_SCENE_LEVEL | data[2]);
    logDebugP("Value %i", value);

    resultData[0] = value == 0xFF ? 0xFF : DaliHelper::arcToPercent(value);

    if(value != 0xFF && data[3] == PT_deviceType_DT8)
    {
        //colorType is TunableWhite
        if(data[4] == PT_colorType_TW)
        {
            resultLength = 3;
            // sendCmdSpecial(DaliSpecialCmd::SET_DTR, 0xF0);
            // sendCmdSpecial(DaliSpecialCmd::ENABLE_DT, 0x08);
            // int16_t resp = getInfo(data[1], DaliCmdExtendedDT8::QUERY_COLOR_VALUE);
            // uint16_t mirek = resp;
            sendCmdSpecial(DaliSpecialCmd::SET_DTR, 0xE2);
            sendCmdSpecial(DaliSpecialCmd::ENABLE_DT, 0x08);
            uint16_t mirek = getInfo(data[1], DaliCmdExtendedDT8::QUERY_COLOR_VALUE) << 8;
            mirek |= getInfo(data[1], DaliCmd::QUERY_DTR);
            logDebugP("mirek %i", mirek);

            uint16_t kelvin = 1000000.0 / mirek;

            resultData[1] = (kelvin >> 8) & 0xFF;
            resultData[2] = kelvin & 0xFF;
            logDebugP("Scene %i: %.1f%% TEMP=%iK", data[2], DaliHelper::arcToPercentFloat(value), kelvin);
        } else { //it is RGB
            resultLength = 4;
            sendCmdSpecial(DaliSpecialCmd::SET_DTR, 0xE9);
            sendCmdSpecial(DaliSpecialCmd::ENABLE_DT, 0x08);
            uint8_t colorVal = getInfo(data[1], DaliCmdExtendedDT8::QUERY_COLOR_VALUE);
            resultData[1] = colorVal;
            sendCmdSpecial(DaliSpecialCmd::SET_DTR, 0xEA);
            sendCmdSpecial(DaliSpecialCmd::ENABLE_DT, 0x08);
            colorVal = getInfo(data[1], DaliCmdExtendedDT8::QUERY_COLOR_VALUE);
            resultData[2] = colorVal;
            sendCmdSpecial(DaliSpecialCmd::SET_DTR, 0xEB);
            sendCmdSpecial(DaliSpecialCmd::ENABLE_DT, 0x08);
            colorVal = getInfo(data[1], DaliCmdExtendedDT8::QUERY_COLOR_VALUE);
            resultData[3] = colorVal;
            logDebugP("Scene %i: %.1f%% RGB=%.2X%.2X%.2X", data[2], DaliHelper::arcToPercentFloat(value), resultData[1], resultData[2], resultData[3]);
        }
    } else {
        resultLength = 1;
        logDebugP("Scene %i: %.1f%%", data[2], DaliHelper::arcToPercentFloat(value));
    }
}

void DaliModule::funcHandleIdentify(uint8_t *data, uint8_t *resultData, uint8_t &resultLength)
{
    sendCmd(0xFF, DaliCmd::OFF, 0xFF);
    sendCmd(data[1], DaliCmd::RECALL_MAX);
    resultLength = 0;
}

bool DaliModule::processFunctionPropertyState(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t *data, uint8_t *resultData, uint8_t &resultLength)
{
    if(objectIndex != 160 || propertyId != 1) return false;

    switch(data[0])
    {
        case 3:
        case 5:
            stateHandleScanAndAddress(data, resultData, resultLength);
            return true;

        case 4:
            stateHandleAssign(data, resultData, resultLength);
            return true;

        case 7:
            stateHandleFoundEVGs(data, resultData, resultLength);
            return true;
    }
    return false;
}

void DaliModule::stateHandleType(uint8_t *data, uint8_t *resultData, uint8_t &resultLength)
{
    int16_t resp = queue.getResponse(data[1]);

    if(resp == -255)
    {
        resultData[0] = 0x00;
        resultLength = 1;
    } else {
        logInfoP("Send Response %i", resp);
        if(resp >= 0)
        {
            resultData[0] = 0x01;
            resultData[1] = (uint8_t)resp;
        } else {
            resultData[0] = 0x02;
            resultData[1] = (uint8_t)(resp*-1);
        }
        resultLength = 2;
    }
}

void DaliModule::stateHandleAssign(uint8_t *data, uint8_t *resultData, uint8_t &resultLength)
{
    resultData[0] = (uint8_t)_assState;
    resultLength = 1;
}

void DaliModule::stateHandleScanAndAddress(uint8_t *data, uint8_t *resultData, uint8_t &resultLength)
{
    resultData[0] = _adrState == AddressingState::None;
    if(data[0] == 3)
    {
        resultData[1] = _adrFound;
        resultLength = 2;
    } else {
        resultLength = 1;
    }
}

void DaliModule::stateHandleFoundEVGs(uint8_t *data, uint8_t *resultData, uint8_t &resultLength)
{
    if(data[1] == 254)
    {
        delete[] ballasts;
        delete[] addresses;
        resultLength = 0;
        _adrState = AddressingState::None;
        _assState = AssigningState::None;
        return;
    }

    resultData[0] = data[1] < _adrFound;
    if(data[1] < _adrFound)
    {
        resultData[1] = ballasts[data[1]].high;
        resultData[2] = ballasts[data[1]].middle;
        resultData[3] = ballasts[data[1]].low;
        resultData[4] = ballasts[data[1]].address;
        resultLength = 5;
    } else {
        resultLength = 1;
    }
}

uint8_t DaliModule::sendArc(byte addr, byte value, byte type)
{
    Message *msg = new Message();
    msg->id = queue.getNextId();
    msg->type = MessageType::Arc;
    msg->para1 = addr;
    msg->para2 = DaliHelper::percentToArc(value);
    msg->addrtype = type;
    return queue.push(msg);
}

uint8_t DaliModule::sendCmd(byte addr, byte value, byte type, bool wait)
{
    Message *msg = new Message();
    msg->id = queue.getNextId();
    msg->type = MessageType::Cmd;
    msg->para1 = addr;
    msg->para2 = value;
    msg->addrtype = type;
    msg->wait = wait;
    return queue.push(msg);
}

uint8_t DaliModule::sendCmdSpecial(DaliSpecialCmd command, byte value, bool wait)
{
    Message *msg = new Message();
    msg->id = queue.getNextId();
    msg->type = MessageType::SpecialCmd;
    msg->para1 = static_cast<uint8_t>(command);
    msg->para2 = value;
    msg->addrtype = 0;
    msg->wait = wait;
    return queue.push(msg);
}

uint8_t DaliModule::sendMsg(MessageType t, byte p1, byte p2, byte type, bool wait)
{
    Message *msg = new Message();
    msg->id = queue.getNextId();
    msg->type = t;
    msg->para1 = p1;
    msg->para2 = p2;
    msg->addrtype = type;
    msg->wait = wait;
    return queue.push(msg);
}

DaliModule openknxDaliModule;
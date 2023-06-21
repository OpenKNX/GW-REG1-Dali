#include "DaliModule.h"

const std::string DaliModule::name()
{
    return "Dali";
}

//You can also give it a version
//will be displayed in Command Infos 
const std::string DaliModule::version()
{
    return "0.0dev";
}

bool DaliModule::usesDualCore()
{
    return true;
}

//will be called once
//only if knx.configured == true
void DaliModule::setup()
{
    dali = new DaliClass();
	dali->begin(DALI_TX, DALI_RX);
    
    queue = new MessageQueue();

    for(int i = 0; i < 64; i++)
    {
        DaliChannel *ch = new DaliChannel(i, queue, false);
        ch->setup();
        channels[i] = ch;
    }

    
    for(int i = 0; i < 16; i++)
    {
        DaliChannel *ch = new DaliChannel(i, queue, true);
        ch->setup();
        groups[i] = ch;
    }
}

void DaliModule::loop()
{
    if(_adrState != AddressingState::None) return;

    for(int i = 0; i < 64; i++)
    {
        channels[i]->loop();
    }
    for(int i = 0; i < 16; i++)
    {
        groups[i]->loop();
    }
}

void DaliModule::loop1()
{
    if(_adrState != AddressingState::None)
    {
        loopAddressing();
        loopMessages();
        return;
    }

    for(int i = 0; i < 64; i++)
    {
        channels[i]->loop1();
    }
    for(int i = 0; i < 16; i++)
    {
        groups[i]->loop1();
    }

    loopMessages();
}

void DaliModule::loopMessages()
{
    Message *msg = queue->pop();
    if(msg == nullptr) return;

    switch(msg->type)
    {
        case MessageType::Arc:
        {
            int16_t resp = dali->sendArcWait(msg->addr, msg->value, msg->addrtype);
            if(msg->wait)
                queue->setResponse(msg->id, resp);
            break;
        }

        case MessageType::Cmd:
        {
            logInfoP("Send CMD %i", msg->value);
            int16_t resp = dali->sendCmdWait(msg->addr, msg->value, msg->addrtype);
            if(msg->wait)
            {
                //logInfoP("Got Response %i = %i", msg->id, resp);
                queue->setResponse(msg->id, resp);
            }
            break;
        }

        case MessageType::SpecialCmd:
        {
            int16_t resp = dali->sendSpecialCmdWait(msg->addr, msg->value);
            if(msg->wait)
            {
                //logInfoP("Got Response %i = %i", msg->id, resp);
                queue->setResponse(msg->id, resp);
            }
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
            if(millis() - _adrTime > 1000)
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
                //logInfoP("No more ballasts");
                _adrState = AddressingState::Finish;
                break;
            }

            //logInfoP("Search: %i - %i", _adrLow, _adrHigh);
            byte high = _adrHigh >> 16;
            byte middle = (_adrHigh >> 8) & 0xFF;
            byte low = _adrHigh & 0xFF;
            sendMsg(MessageType::SpecialCmd, dali->CMD_SEARCHADDRH, false, high);
            sendMsg(MessageType::SpecialCmd, dali->CMD_SEARCHADDRM, false, middle);
            sendMsg(MessageType::SpecialCmd, dali->CMD_SEARCHADDRL, false, low);
            _adrResp = sendMsg(MessageType::SpecialCmd, dali->CMD_COMPARE, false, 0x00, true);
            _adrTime = millis();
            _adrState = AddressingState::SearchWait;
            break;
        }

        case AddressingState::SearchWait:
        {
            int16_t response = queue->getResponse(_adrResp);
            if(response == -200 || response == -1)
            {
                if(millis() - _adrTime > SEARCHWAIT_TIME)
                {
                    _adrLow = _adrHigh + 1;
                    _adrHigh = _adrHighLast;
                    _adrState = AddressingState::Search;
                    _adrNoRespCounter++;
                }
            } else if(response >= 0) {
                if(_adrLow == _adrHigh)
                {
                    logInfoP("Found Ballast at %X", _adrLow);
                    _adrResp = sendMsg(MessageType::SpecialCmd, dali->CMD_QUERY_SHORT, false, 0, true);
                    _adrTime = millis();
                    _adrState = AddressingState::Found;
                } else {
                    //logInfoP("Range has ballast");
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
            int16_t response = queue->getResponse(_adrResp);
            if(response == -255)
            {
                if(millis() - _adrTime > SEARCHWAIT_TIME)
                {
                    logErrorP("Found ballast not answering");
                    _adrState = AddressingState::Finish;
                }
            } else if(response >= 0) {
                logInfoP("Ballast has Short Address %i", response);

                ballasts[_adrFound].high = (_adrLow >> 16) & 0xFF;
                ballasts[_adrFound].middle = (_adrLow >> 8) & 0xFF;
                ballasts[_adrFound].low = _adrLow & 0xFF;
                ballasts[_adrFound].address = response;
                _adrFound++;

                _adrLow = 0;
                _adrHigh = 0xFFFFFF;
                _adrHighLast = 0xFFFFFF;
                _adrNoRespCounter = 1;
                sendMsg(MessageType::SpecialCmd, dali->CMD_WITHDRAW, false, 0x00);
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
            sendMsg(MessageType::SpecialCmd, dali->CMD_TERMINATE, false, 0x00);
            sendMsg(MessageType::SpecialCmd, dali->CMD_TERMINATE, false, 0x00);
            _adrState = AddressingState::None;
            break;
        }
    }
}

bool DaliModule::getDaliBusState()
{
    return _daliBusState;
}

void DaliModule::processInputKo(GroupObject &ko)
{
    if(_adrState != AddressingState::None) return;

    int koNum = ko.asap();
    if(koNum >= ADR_KoOffset && koNum < ADR_KoOffset + ADR_KoBlockSize * 64)
    {
        int index = floor((koNum - ADR_KoOffset) / ADR_KoBlockSize);
        logInfoP("Got KO %i for CH%i", koNum, index);
        channels[index]->processInputKo(ko);
    }

    if(koNum >= GRP_KoOffset && koNum < GRP_KoOffset + GRP_KoBlockSize * 16)
    {
        int index = floor((koNum - GRP_KoOffset) / GRP_KoBlockSize);
        logInfoP("Got KO %i for G%i", koNum, index);
        groups[index]->processInputKo(ko);
    }
}

bool DaliModule::processFunctionProperty(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t *data, uint8_t *resultData, uint8_t &resultLength)
{
    if(objectIndex != 164) return false;

    switch(propertyId)
    {
        case 2:
        {
            uint8_t resp = sendMsg(MessageType::Cmd, data[0], false, dali->CMD_QUERY_DEVICE_TYPE, true);
        
            //todo handle error if dali is not present

            resultData[0] = 0x00;
            resultData[1] = resp;
            resultLength = 2;
            return true;
        }

        case 3:
        {
            ballasts = new Ballast[64];
            logInfoP("Starting Addressing");
            _adrState = AddressingState::Init;
            _adrLow = 0;
            _adrHigh = 0xFFFFFF;
            _adrHighLast = 0xFFFFFF;
            _adrFound = 0;

            sendMsg(MessageType::SpecialCmd, dali->CMD_INITIALISE, false, 0);
            sendMsg(MessageType::SpecialCmd, dali->CMD_INITIALISE, false, 0);
            sendMsg(MessageType::SpecialCmd, dali->CMD_RANDOMISE, false, 0);
            sendMsg(MessageType::SpecialCmd, dali->CMD_RANDOMISE, false, 0);
            logInfoP("RandomizeWait");

            _adrState = AddressingState::Randomize_Wait;
            _adrTime = millis();
            _adrNoRespCounter = 1;

            resultLength = 0;
            return true;
        }
    }


    return false;
}

bool DaliModule::processFunctionPropertyState(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t *data, uint8_t *resultData, uint8_t &resultLength)
{
    if(objectIndex != 164) return false;

    switch(propertyId)
    {
        case 1:
        {
            int16_t resp = queue->getResponse(data[0]);
            logInfoP("Check Response %i = %i", data[0], resp);

            if(resp == -255)
            {
                resultData[0] = 0x00;
                resultLength = 1;
                return true;
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
                return true;
            }
        }

        case 3:
        {
            resultData[0] = data[0] < _adrFound;
            resultData[1] = (data[0] >= _adrFound) && _adrState == AddressingState::None;
            if(data[0] < _adrFound)
            {
                resultData[2] = ballasts[data[0]].high;
                resultData[3] = ballasts[data[0]].middle;
                resultData[4] = ballasts[data[0]].low;
                resultData[5] = ballasts[data[0]].address;
                resultLength = 6;
            } else {
                resultLength = 2;
            }

            if(resultData[1])
            {
                delete[] ballasts;
            }

            return true;
        }
    }
    return false;
}

uint8_t DaliModule::sendMsg(MessageType t, byte addr, bool isGroup, byte v, bool wait)
{
    Message *msg = new Message();
    msg->id = queue->getNextId();
    msg->type = t;
    msg->addr = addr;
    msg->addrtype = isGroup;
    msg->value = v;
    msg->wait = wait;
    return queue->push(msg);
}

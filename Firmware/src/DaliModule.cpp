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
    for(int i = 0; i < 64; i++)
    {
        channels[i]->loop1();
    }
    for(int i = 0; i < 16; i++)
    {
        groups[i]->loop1();
    }

/*
    bool daliState = digitalRead(DALI_RX);

    if(daliState != _daliBusState)
    {
        _daliStateCounter = 0;
        _daliBusState = daliState;
    } else {
        if(_daliStateCounter == 1000)
        {
            if(_daliBusState)
                logErrorP("DALI Bus is not connected");
            else
                logInfoP("DALI Bus is connected");
        }
        if(_daliStateCounter < 1010)
            _daliStateCounter++;
    }
    */

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
                logInfoP("Got Response %i = %i", msg->id, resp);
                queue->setResponse(msg->id, resp);
            }
            break;
        }
    }

    delete[] msg;
}

bool DaliModule::getDaliBusState()
{
    return _daliBusState;
}

void DaliModule::processInputKo(GroupObject &ko)
{
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
    }


    return false;
}

bool DaliModule::processFunctionPropertyState(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t *data, uint8_t *resultData, uint8_t &resultLength)
{
    logInfoP("Got FunctionPropertyState %i %i", objectIndex, propertyId);
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

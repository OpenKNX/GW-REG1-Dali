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
        bool type = ParamADR_typeIndex(i);
        if(type)
        {
            logInfoP("CH%i Treppenhaus", i);
            StaircaseChannel *ch = new StaircaseChannel(i, queue, false);
            ch->setInterval(ParamADR_stairtimeIndex(i));
            ch->setMin((ParamADR_minIndex(i) * 2.54));
            ch->setMax((ParamADR_maxIndex(i) * 2.54));
            channels[i] = ch;
        } else {
            logInfoP("CH%i Normalbetrieb", i);
            channels[i] = new StandardChannel(i, queue, false);
        }
    }

    
    for(int i = 0; i < 16; i++)
    {
        bool type = ParamGRP_typeIndex(i);
        if(type)
        {
            logInfoP("GR%i Treppenhaus", i);
            StaircaseChannel *ch = new StaircaseChannel(i, queue, true);
            ch->setInterval(ParamGRP_stairtimeIndex(i)); //todo replace with group settings
            ch->setMin(0); //TODO check what cockpit does with this
            ch->setMax(254);
            groups[i] = ch;
        } else {
            logInfoP("GR%i Normalbetrieb", i);
            groups[i] = new StandardChannel(i, queue, true);
        }
    }
}

void DaliModule::loop()
{
    for(int i = 0; i < 64; i++)
    {
        channels[i]->loop();
    }
}

void DaliModule::loop1()
{
    for(int i = 0; i < 64; i++)
    {
        channels[i]->loop1();
    }

    bool daliState = digitalRead(DALI_RX);
    if(daliState)
    {
        if(!_daliBusState)
        {
            logInfoP("DALI Bus is connected");
        }
        _daliBusState = true;
        _daliStateCounter = 0;
    } else {
        _daliStateCounter++;
        if(_daliStateCounter == 10)
        {
            logErrorP("DALI Bus is not connected");
        }
    }

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
    if(koNum >= ADR_KoOffset && koNum <= ADR_KoOffset + ADR_KoBlockSize * 64)
    {
        int index = floor((koNum - ADR_KoOffset) / ADR_KoBlockSize);
        logInfoP("Got KO %i for CH%i", koNum, index);
        channels[index]->processInputKo(ko);
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

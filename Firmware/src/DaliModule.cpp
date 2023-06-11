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
	dali->begin(17, 16);

    for(int i = 0; i < 64; i++)
    {
        bool type = ParamADR_typeIndex(i);
        if(type)
        {
            logInfoP("CH%i Treppenhaus", i);
            channels[i] = new StaircaseChannel(i, dali);
        } else {
            logInfoP("CH%i Normalbetrieb", i);
            channels[i] = new StandardChannel(i, dali);
        }
    }
}

void DaliModule::loop()
{
}

void DaliModule::loop1()
{
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
    return false;
}

bool DaliModule::processFunctionPropertyState(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t *data, uint8_t *resultData, uint8_t &resultLength)
{
    return false;
}
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

    bool *types = new bool[64];
    for(int i = 0; i < 64; i++)
    {
        bool type = ParamADR_typeIndex(i);
        if(type)
            staircaseCount++;
        else
            standardCount++;

        types[i] = type;
    }
    
    standards = new StandardChannel*[standardCount];
    staircases = new StaircaseChannel*[staircaseCount];

    int count1 = 0;
    int count2 = 0;
    for(int i = 0; i < 64; i++)
    {
        if(types[i])
        {
            logInfoP("CH%i Treppenhaus", i);
            staircases[count2] = new StaircaseChannel(i);
            count1++;
        } else {
            logInfoP("CH%i Normalbetrieb", i);
            standards[count1] = new StandardChannel(i);
            count1++;
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

}

bool DaliModule::processFunctionProperty(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t *data, uint8_t *resultData, uint8_t &resultLength)
{
    return false;
}

bool DaliModule::processFunctionPropertyState(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t *data, uint8_t *resultData, uint8_t &resultLength)
{
    return false;
}
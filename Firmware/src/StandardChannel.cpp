#include "StandardChannel.h"

StandardChannel::StandardChannel(uint8_t channelIndex, DaliClass *dali)
{
    _channelIndex = channelIndex;
    _dali = dali;
}

StandardChannel::~StandardChannel()
{
}

const std::string StandardChannel::name()
{
    return "StandardChannel";
}


//will be called once
//only if knx.configured == true
void StandardChannel::setup()
{
}

void StandardChannel::loop()
{
}

void StandardChannel::loop1()
{
}

void StandardChannel::processInputKo(GroupObject &ko)
{
    int chanIndex = (ko.asap() - ADR_KoOffset) % ADR_KoBlockSize;
    logInfoP("Got KO %i", chanIndex);
    switch(chanIndex)
    {
        case 0:
        {
            if(ko.value(DPT_Switch))
            {
                logInfoP("Einschalten");
                _dali->sendArc(_channelIndex, 0xFE);
            }
            else
            {
                logInfoP("Ausschalten");
                _dali->sendArc(_channelIndex, 0);
            }
            break;
        }

        case 2:
        {
            logInfoP("Dimmen relativ");
            break;
        }

        case 3:
        {
            logInfoP("Dimmen Absolut auf %i%%", ko.value(Dpt(5,1)));
            break;
        }
    }
}
#include "StaircaseChannel.h"

StaircaseChannel::StaircaseChannel(uint8_t channelIndex)
{
    _channelIndex = channelIndex;
}

StaircaseChannel::~StaircaseChannel()
{
}

const std::string StaircaseChannel::name()
{
    return "StaircaseChannel";
}


//will be called once
//only if knx.configured == true
void StaircaseChannel::setup()
{
}

void StaircaseChannel::loop()
{
}

void StaircaseChannel::loop1()
{
}

void StaircaseChannel::processInputKo(GroupObject &ko)
{
}
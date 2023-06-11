#include "StandardChannel.h"

StandardChannel::StandardChannel(uint8_t channelIndex)
{
    _channelIndex = channelIndex;
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
}
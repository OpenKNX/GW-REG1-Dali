#include "Frame.h"

LubaFrame::LubaFrame(uint8_t *data)
{
    _data = data;
}

uint8_t LubaFrame::command()
{
    return _data[1];
}

uint8_t LubaFrame::length()
{
    return _data[2];
}
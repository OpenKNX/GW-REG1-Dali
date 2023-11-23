#include "DaliHelper.h"

uint8_t DaliHelper::percentToArc(uint8_t value)
{
    if(value == 0)
    {
        return 0;
    }
    //Todo also include _max
    uint8_t arc = roundToInt(((253/3.0)*(log10(value)+1)) + 1);
    return arc;
}

uint8_t DaliHelper::arcToPercent(uint8_t value)
{
    if(value == 0)
    {
        return 0;
    }
    //Todo also include _max
    double arc = pow(10, ((value-1) / (253/3.0)) - 1);
    return roundToInt(arc);;
}

uint8_t DaliHelper::roundToInt(double input)
{
    double temp = input + 0.5;
    return (uint8_t)temp;
}

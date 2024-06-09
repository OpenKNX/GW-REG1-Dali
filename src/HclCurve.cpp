#include "HclCurve.h"

const std::string HclCurve::logPrefix()
{
    std::string name = "HCL<";
    name += std::to_string(_index+1);
    name += ">";
    return name;
}

const uint8_t HclCurve::channelIndex()
{
    return _index;
}

void HclCurve::setup(uint8_t index)
{
    _index = index;
    _type = ParamHCL_type;
    _isConfigured = _type != PT_hclType_none;
    if(_isConfigured)
    {
        if(_type == PT_hclType_sun)
            logDebugP("Konfiguriert: Sonnenstand %i/%i K - %i/%i %%", ParamHCL_min, ParamHCL_max, ParamHCL_briMin, ParamHCL_briMax);
        else if(_type == PT_hclType_time)
            logDebugP("Konfiguriert: Zeittabelle");
    } else 
        logDebugP("Nicht Konfiguriert");

    _lastCheck = _index*3000;
}

void HclCurve::loop()
{
    if(!_isConfigured) return;

    sTime *sunRise = openknxTimerModule.getSunInfo(SUN_SUNRISE);
    sTime *sunSet = openknxTimerModule.getSunInfo(SUN_SUNSET);

    if((sunRise->hour == 0 && sunRise->minute == 0) || (sunSet->hour == 0 && sunSet->minute == 0))
    {
        logDebugP("Ungueltige Sonnenstandsdaten");
        return;
    }
        
    logDebugP("Aktuelle Zeit: %i:%i:%i", openknxTimerModule.getHour(), openknxTimerModule.getMinute(), openknxTimerModule.getSecond());

    uint16_t minT = ParamHCL_min;
    uint8_t minB = ParamHCL_briMin;

    if(openknxTimerModule.getHour() < sunRise->hour || (openknxTimerModule.getHour() == sunRise->hour && openknxTimerModule.getMinute() < sunRise->minute))
    {
        logDebugP("Vor Sonnenaufgang %i K (%i:%i)", minT, sunRise->hour, sunRise->minute);
        if(ParamHCL_checkTemperature)
            KoHCL_hcl_state.value(minT, Dpt(7, 600));
        if(ParamHCL_checkBrightness)
            KoHCL_bri_state.value(minB, Dpt(5, 1));
    } else if(openknxTimerModule.getHour() > sunSet->hour || (openknxTimerModule.getHour() == sunSet->hour && openknxTimerModule.getMinute() > sunSet->minute)) {
        logDebugP("Nach Sonnenuntergang %i K (%i:%i)", minT, sunSet->hour, sunSet->minute);
        if(ParamHCL_checkTemperature)
            KoHCL_hcl_state.value(minT, Dpt(7, 600));
        if(ParamHCL_checkBrightness)
            KoHCL_bri_state.value(minB, Dpt(5, 1));
    } else {
        logDebugP("Dazwischen %i:%i - jetzt - %i:%i", sunRise->hour, sunRise->minute, sunSet->hour, sunSet->minute);
        uint16_t startMin = sunRise->hour*60 + sunRise->minute;
        uint16_t stopMin = sunSet->hour*60 + sunSet->minute;

        if(ParamHCL_offsetRiseType == PT_offset_plus)
            startMin += ParamHCL_offsetRiseMin;
        else if(ParamHCL_offsetRiseType == PT_offset_minus)
            startMin -= ParamHCL_offsetRiseMin;

        if(ParamHCL_offsetSetType == PT_offset_plus)
            stopMin += ParamHCL_offsetSetMin;
        else if(ParamHCL_offsetSetType == PT_offset_minus)
            stopMin -= ParamHCL_offsetSetMin;

        uint16_t currentMin = openknxTimerModule.getHour()*60 + openknxTimerModule.getMinute();
        //logDebugP("start %i | stop %i | curr %i", startMin, stopMin, currentMin);
        uint16_t response = 0;
        uint16_t maxT = ParamHCL_max;
        uint8_t maxB = ParamHCL_briMax;
        
        if(ParamHCL_checkTemperature)
        {
            response = ColorHelper::getValueFromSun(currentMin - startMin, stopMin - startMin, minT, maxT);
            logDebugP("response: %i K", response);
            KoHCL_hcl_state.value(response, Dpt(7, 600));
        }
        if(ParamHCL_checkBrightness)
        {
            response = ColorHelper::getValueFromSun(currentMin - startMin, stopMin - startMin, minB, maxB);
            logDebugP("response: %i %", response);
            KoHCL_bri_state.value(response, Dpt(5, 1));
        }
    }
}
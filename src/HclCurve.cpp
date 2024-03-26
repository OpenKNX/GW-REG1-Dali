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
            logDebugP("Konfiguriert: Sonnenstand %i/%i", ParamHCL_min, ParamHCL_max);
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

    uint16_t min = ParamHCL_min;
    uint16_t max = ParamHCL_max;
    if(openknxTimerModule.getHour() < sunRise->hour || (openknxTimerModule.getHour() == sunRise->hour && openknxTimerModule.getMinute() < sunRise->minute))
    {
        logDebugP("Vor Sonnenaufgang %i K", min);
        KoHCL_hcl_state.value(min, Dpt(7, 600));
        // if(KoHCL_hcl_state.valueNoSendCompare(min, Dpt(7, 600)))
        //     KoHCL_hcl_state.objectWritten();
    } else if(openknxTimerModule.getHour() > sunSet->hour || (openknxTimerModule.getHour() == sunSet->hour && openknxTimerModule.getMinute() > sunSet->minute)) {
        logDebugP("Nach Sonnenuntergang %i K (%i:%i)", max, sunSet->hour, sunSet->minute);
        KoHCL_hcl_state.value(max, Dpt(7, 600));
        // if(KoHCL_hcl_state.valueNoSendCompare(max, Dpt(7, 600)))
        //     KoHCL_hcl_state.objectWritten();
    } else {
        logDebugP("Irgendwas dazwischen");
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
        logDebugP("start %i | stop %i | curr %i", startMin, stopMin, currentMin);
        uint16_t response = ColorHelper::getKelvinFromSun(currentMin - startMin, stopMin - startMin, min, max);
        logDebugP("response: %i K", response);
        KoHCL_hcl_state.value(response, Dpt(7, 600));
    }
}
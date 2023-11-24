#pragma once

#define paramDelay(time) (uint32_t)( \
            (time & 0xC000) == 0xC000 ? (time & 0x3FFF) * 100 : \
            (time & 0xC000) == 0x0000 ? (time & 0x3FFF) * 1000 : \
            (time & 0xC000) == 0x4000 ? (time & 0x3FFF) * 60000 : \
            (time & 0xC000) == 0x8000 ? ((time & 0x3FFF) > 1000 ? 3600000 : \
                                            (time & 0x3FFF) * 3600000 ) : 0 )
//--------------------Allgemein---------------------------
#define MAIN_OpenKnxId 0xA4
#define MAIN_ApplicationNumber 0x01
#define MAIN_ApplicationVersion 0x00
#define MAIN_OrderNumber "GW-REG1-Dali.01"
#define MAIN_ParameterSize 1089
#define MAIN_MaxKoNumber 1269


#define APP_daynight		0x0000
// Offset: 0, Size: 1 Bit, Text: Tag/Nacht Objekt
#define ParamAPP_daynight knx.paramBit(0, 0)
//!< Number: 1, Text: Broadcast, Function: Schalten
#define APP_Kobroadcast_switch 1
#define KoAPP_broadcast_switch knx.getGroupObject(1)
//!< Number: 2, Text: Broadcast, Function: Dimmen Absolut
#define APP_Kobroadcast_dimm 2
#define KoAPP_broadcast_dimm knx.getGroupObject(2)
//!< Number: 3, Text: Allgemein, Function: Tag/Nacht
#define APP_Kodaynight 3
#define KoAPP_daynight knx.getGroupObject(3)
//!< Number: 4, Text: Allgemein, Function: Einschaltwert Tag
#define APP_KoonValue 4
#define KoAPP_onValue knx.getGroupObject(4)
//!< Number: 5, Text: Allgemein, Function: Szene
#define APP_Koscene 5
#define KoAPP_scene knx.getGroupObject(5)

//---------------------Modules----------------------------

//-----Module specific starts
#define SCE_ParamBlockOffset 1
#define SCE_ParamBlockSize 4
#define ADR_ParamBlockOffset 65
#define ADR_ParamBlockSize 14
#define GRP_ParamBlockOffset 961
#define GRP_ParamBlockSize 8
#define SCE_KoOffset 6
#define SCE_KoBlockSize 0
#define ADR_KoOffset 22
#define ADR_KoBlockSize 18
#define GRP_KoOffset 1174
#define GRP_KoBlockSize 6

//-----Module: adresse
#define ADR_deviceType		0x0000
#define ADR_deviceType_Shift	4
#define ADR_deviceType_Mask	0x000F
// Offset: 0, Size: 4 Bit, Text: Gerätetyp
#define ParamADR_deviceTypeIndex(X) ((uint)((knx.paramByte((ADR_ParamBlockOffset + ADR_ParamBlockSize * X + 0)) >> ADR_deviceType_Shift) & ADR_deviceType_Mask))
// Offset: 0, Size: 4 Bit, Text: Gerätetyp
#define ParamADR_deviceType ((uint)((knx.paramByte((ADR_ParamBlockOffset + ADR_ParamBlockSize * channelIndex() + 0)) >> ADR_deviceType_Shift) & ADR_deviceType_Mask))
#define ADR_type		0x0000
// Offset: 0, BitOffset: 4, Size: 1 Bit, Text: Betriebsart
#define ParamADR_typeIndex(X) knx.paramBit((ADR_ParamBlockOffset + ADR_ParamBlockSize * X + 0), 4)
// Offset: 0, BitOffset: 4, Size: 1 Bit, Text: Betriebsart
#define ParamADR_type knx.paramBit((ADR_ParamBlockOffset + ADR_ParamBlockSize * channelIndex() + 0), 4)
#define ADR_error		0x0000
// Offset: 0, BitOffset: 5, Size: 1 Bit, Text: Fehlerstatus auslesen
#define ParamADR_errorIndex(X) knx.paramBit((ADR_ParamBlockOffset + ADR_ParamBlockSize * X + 0), 5)
// Offset: 0, BitOffset: 5, Size: 1 Bit, Text: Fehlerstatus auslesen
#define ParamADR_error knx.paramBit((ADR_ParamBlockOffset + ADR_ParamBlockSize * channelIndex() + 0), 5)
#define ADR_min		0x0001
#define ADR_min_Shift	1
#define ADR_min_Mask	0x007F
// Offset: 1, Size: 7 Bit, Text: 
#define ParamADR_minIndex(X) ((uint)((knx.paramByte((ADR_ParamBlockOffset + ADR_ParamBlockSize * X + 1)) >> ADR_min_Shift) & ADR_min_Mask))
// Offset: 1, Size: 7 Bit, Text: 
#define ParamADR_min ((uint)((knx.paramByte((ADR_ParamBlockOffset + ADR_ParamBlockSize * channelIndex() + 1)) >> ADR_min_Shift) & ADR_min_Mask))
#define ADR_max		0x0002
#define ADR_max_Shift	1
#define ADR_max_Mask	0x007F
// Offset: 2, Size: 7 Bit, Text: 
#define ParamADR_maxIndex(X) ((uint)((knx.paramByte((ADR_ParamBlockOffset + ADR_ParamBlockSize * X + 2)) >> ADR_max_Shift) & ADR_max_Mask))
// Offset: 2, Size: 7 Bit, Text: 
#define ParamADR_max ((uint)((knx.paramByte((ADR_ParamBlockOffset + ADR_ParamBlockSize * channelIndex() + 2)) >> ADR_max_Shift) & ADR_max_Mask))
#define ADR_nachtriggern		0x0000
// Offset: 0, BitOffset: 6, Size: 1 Bit, Text: Nachtriggern erlauben
#define ParamADR_nachtriggernIndex(X) knx.paramBit((ADR_ParamBlockOffset + ADR_ParamBlockSize * X + 0), 6)
// Offset: 0, BitOffset: 6, Size: 1 Bit, Text: Nachtriggern erlauben
#define ParamADR_nachtriggern knx.paramBit((ADR_ParamBlockOffset + ADR_ParamBlockSize * channelIndex() + 0), 6)
#define ADR_stairtime		0x0003
#define ADR_stairtime_Shift	5
#define ADR_stairtime_Mask	0x07FF
// Offset: 3, Size: 11 Bit, Text: Nachlaufzeit
#define ParamADR_stairtimeIndex(X) ((uint)((knx.paramWord((ADR_ParamBlockOffset + ADR_ParamBlockSize * X + 3)) >> ADR_stairtime_Shift) & ADR_stairtime_Mask))
// Offset: 3, Size: 11 Bit, Text: Nachlaufzeit
#define ParamADR_stairtime ((uint)((knx.paramWord((ADR_ParamBlockOffset + ADR_ParamBlockSize * channelIndex() + 3)) >> ADR_stairtime_Shift) & ADR_stairtime_Mask))
#define ADR_manuoff		0x0000
// Offset: 0, BitOffset: 7, Size: 1 Bit, Text: Manuelles ausschalten
#define ParamADR_manuoffIndex(X) knx.paramBit((ADR_ParamBlockOffset + ADR_ParamBlockSize * X + 0), 7)
// Offset: 0, BitOffset: 7, Size: 1 Bit, Text: Manuelles ausschalten
#define ParamADR_manuoff knx.paramBit((ADR_ParamBlockOffset + ADR_ParamBlockSize * channelIndex() + 0), 7)
#define ADR_lockbehave		0x0005
#define ADR_lockbehave_Shift	6
#define ADR_lockbehave_Mask	0x0003
// Offset: 5, Size: 2 Bit, Text: Verhalten bei Sperren
#define ParamADR_lockbehaveIndex(X) ((uint)((knx.paramByte((ADR_ParamBlockOffset + ADR_ParamBlockSize * X + 5)) >> ADR_lockbehave_Shift) & ADR_lockbehave_Mask))
// Offset: 5, Size: 2 Bit, Text: Verhalten bei Sperren
#define ParamADR_lockbehave ((uint)((knx.paramByte((ADR_ParamBlockOffset + ADR_ParamBlockSize * channelIndex() + 5)) >> ADR_lockbehave_Shift) & ADR_lockbehave_Mask))
#define ADR_lockvalue		0x0006
#define ADR_lockvalue_Shift	1
#define ADR_lockvalue_Mask	0x007F
// Offset: 6, Size: 7 Bit, Text: 
#define ParamADR_lockvalueIndex(X) ((uint)((knx.paramByte((ADR_ParamBlockOffset + ADR_ParamBlockSize * X + 6)) >> ADR_lockvalue_Shift) & ADR_lockvalue_Mask))
// Offset: 6, Size: 7 Bit, Text: 
#define ParamADR_lockvalue ((uint)((knx.paramByte((ADR_ParamBlockOffset + ADR_ParamBlockSize * channelIndex() + 6)) >> ADR_lockvalue_Shift) & ADR_lockvalue_Mask))
#define ADR_unlockbehave		0x0005
#define ADR_unlockbehave_Shift	4
#define ADR_unlockbehave_Mask	0x0003
// Offset: 5, BitOffset: 2, Size: 2 Bit, Text: Verhalten bei Entsperren
#define ParamADR_unlockbehaveIndex(X) ((uint)((knx.paramByte((ADR_ParamBlockOffset + ADR_ParamBlockSize * X + 5)) >> ADR_unlockbehave_Shift) & ADR_unlockbehave_Mask))
// Offset: 5, BitOffset: 2, Size: 2 Bit, Text: Verhalten bei Entsperren
#define ParamADR_unlockbehave ((uint)((knx.paramByte((ADR_ParamBlockOffset + ADR_ParamBlockSize * channelIndex() + 5)) >> ADR_unlockbehave_Shift) & ADR_unlockbehave_Mask))
#define ADR_unlockvalue		0x0007
#define ADR_unlockvalue_Shift	1
#define ADR_unlockvalue_Mask	0x007F
// Offset: 7, Size: 7 Bit, Text: 
#define ParamADR_unlockvalueIndex(X) ((uint)((knx.paramByte((ADR_ParamBlockOffset + ADR_ParamBlockSize * X + 7)) >> ADR_unlockvalue_Shift) & ADR_unlockvalue_Mask))
// Offset: 7, Size: 7 Bit, Text: 
#define ParamADR_unlockvalue ((uint)((knx.paramByte((ADR_ParamBlockOffset + ADR_ParamBlockSize * channelIndex() + 7)) >> ADR_unlockvalue_Shift) & ADR_unlockvalue_Mask))
#define ADR_onDay		0x0008
#define ADR_onDay_Shift	1
#define ADR_onDay_Mask	0x007F
// Offset: 8, Size: 7 Bit, Text: Einschaltwert Tag
#define ParamADR_onDayIndex(X) ((uint)((knx.paramByte((ADR_ParamBlockOffset + ADR_ParamBlockSize * X + 8)) >> ADR_onDay_Shift) & ADR_onDay_Mask))
// Offset: 8, Size: 7 Bit, Text: Einschaltwert Tag
#define ParamADR_onDay ((uint)((knx.paramByte((ADR_ParamBlockOffset + ADR_ParamBlockSize * channelIndex() + 8)) >> ADR_onDay_Shift) & ADR_onDay_Mask))
#define ADR_onNight		0x0009
#define ADR_onNight_Shift	1
#define ADR_onNight_Mask	0x007F
// Offset: 9, Size: 7 Bit, Text: Einschaltwert Nacht
#define ParamADR_onNightIndex(X) ((uint)((knx.paramByte((ADR_ParamBlockOffset + ADR_ParamBlockSize * X + 9)) >> ADR_onNight_Shift) & ADR_onNight_Mask))
// Offset: 9, Size: 7 Bit, Text: Einschaltwert Nacht
#define ParamADR_onNight ((uint)((knx.paramByte((ADR_ParamBlockOffset + ADR_ParamBlockSize * channelIndex() + 9)) >> ADR_onNight_Shift) & ADR_onNight_Mask))
#define ADR_locknegate		0x0001
// Offset: 1, BitOffset: 7, Size: 1 Bit, Text: Sperren bei
#define ParamADR_locknegateIndex(X) knx.paramBit((ADR_ParamBlockOffset + ADR_ParamBlockSize * X + 1), 7)
// Offset: 1, BitOffset: 7, Size: 1 Bit, Text: Sperren bei
#define ParamADR_locknegate knx.paramBit((ADR_ParamBlockOffset + ADR_ParamBlockSize * channelIndex() + 1), 7)
#define ADR_colorType		0x0005
#define ADR_colorType_Shift	2
#define ADR_colorType_Mask	0x0003
// Offset: 5, BitOffset: 4, Size: 2 Bit, Text: Farbe ansteuern per
#define ParamADR_colorTypeIndex(X) ((uint)((knx.paramByte((ADR_ParamBlockOffset + ADR_ParamBlockSize * X + 5)) >> ADR_colorType_Shift) & ADR_colorType_Mask))
// Offset: 5, BitOffset: 4, Size: 2 Bit, Text: Farbe ansteuern per
#define ParamADR_colorType ((uint)((knx.paramByte((ADR_ParamBlockOffset + ADR_ParamBlockSize * channelIndex() + 5)) >> ADR_colorType_Shift) & ADR_colorType_Mask))
#define ADR_colorSpace		0x0002
// Offset: 2, BitOffset: 7, Size: 1 Bit, Text: Farbe übertragen per
#define ParamADR_colorSpaceIndex(X) knx.paramBit((ADR_ParamBlockOffset + ADR_ParamBlockSize * X + 2), 7)
// Offset: 2, BitOffset: 7, Size: 1 Bit, Text: Farbe übertragen per
#define ParamADR_colorSpace knx.paramBit((ADR_ParamBlockOffset + ADR_ParamBlockSize * channelIndex() + 2), 7)
#define ADR_tempMin		0x000A
// Offset: 10, Size: 16 Bit (2 Byte), Text: Farbtemperatur Min
#define ParamADR_tempMinIndex(X) ((uint)((knx.paramWord((ADR_ParamBlockOffset + ADR_ParamBlockSize * X + 10)))))
// Offset: 10, Size: 16 Bit (2 Byte), Text: Farbtemperatur Min
#define ParamADR_tempMin ((uint)((knx.paramWord((ADR_ParamBlockOffset + ADR_ParamBlockSize * channelIndex() + 10)))))
#define ADR_tempMax		0x000C
// Offset: 12, Size: 16 Bit (2 Byte), Text: Farbtemperatur Max
#define ParamADR_tempMaxIndex(X) ((uint)((knx.paramWord((ADR_ParamBlockOffset + ADR_ParamBlockSize * X + 12)))))
// Offset: 12, Size: 16 Bit (2 Byte), Text: Farbtemperatur Max
#define ParamADR_tempMax ((uint)((knx.paramWord((ADR_ParamBlockOffset + ADR_ParamBlockSize * channelIndex() + 12)))))
#define ADR_hcl		0x0005
// Offset: 5, BitOffset: 6, Size: 1 Bit, Text: HCL aktivieren
#define ParamADR_hclIndex(X) knx.paramBit((ADR_ParamBlockOffset + ADR_ParamBlockSize * X + 5), 6)
// Offset: 5, BitOffset: 6, Size: 1 Bit, Text: HCL aktivieren
#define ParamADR_hcl knx.paramBit((ADR_ParamBlockOffset + ADR_ParamBlockSize * channelIndex() + 5), 6)
//!< Number: 0, Text: A{{argChan}} {{0}}, Function: Schalten
#define ADR_Koswitch 0
#define KoADR_switchIndex(X) knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * X + 0)
#define KoADR_switch knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * channelIndex() + 0)
//!< Number: 1, Text: A{{argChan}} {{0}}, Function: Schalten Status
#define ADR_Koswitch_state 1
#define KoADR_switch_stateIndex(X) knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * X + 1)
#define KoADR_switch_state knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * channelIndex() + 1)
//!< Number: 2, Text: A{{argChan}} {{0}}, Function: Dimmen Relativ
#define ADR_Kodimm_relative 2
#define KoADR_dimm_relativeIndex(X) knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * X + 2)
#define KoADR_dimm_relative knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * channelIndex() + 2)
//!< Number: 3, Text: A{{argChan}} {{0}}, Function: Dimmen Absolut
#define ADR_Kodimm_absolute 3
#define KoADR_dimm_absoluteIndex(X) knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * X + 3)
#define KoADR_dimm_absolute knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * channelIndex() + 3)
//!< Number: 4, Text: A{{argChan}} {{0}}, Function: Dimmen Status
#define ADR_Kodimm_state 4
#define KoADR_dimm_stateIndex(X) knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * X + 4)
#define KoADR_dimm_state knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * channelIndex() + 4)
//!< Number: 5, Text: A{{argChan}} {{0}}, Function: Sperren
#define ADR_Kolock 5
#define KoADR_lockIndex(X) knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * X + 5)
#define KoADR_lock knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * channelIndex() + 5)
//!< Number: 6, Text: A{{argChan}} {{0}}, Function: Fehler
#define ADR_Koerror 6
#define KoADR_errorIndex(X) knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * X + 6)
#define KoADR_error knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * channelIndex() + 6)
//!< Number: 7, Text: A{{argChan}} {{0}}, Function: RGB Farbe
#define ADR_Kocolor 7
#define KoADR_colorIndex(X) knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * X + 7)
#define KoADR_color knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * channelIndex() + 7)
//!< Number: 8, Text: A{{argChan}} {{0}}, Function: RGB Farbe Status
#define ADR_Kocolor_rgb_state 8
#define KoADR_color_rgb_stateIndex(X) knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * X + 8)
#define KoADR_color_rgb_state knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * channelIndex() + 8)
//!< Number: 9, Text: A{{argChan}} {{0}}, Function: Rot Relativ
#define ADR_Kocolor_red_relative 9
#define KoADR_color_red_relativeIndex(X) knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * X + 9)
#define KoADR_color_red_relative knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * channelIndex() + 9)
//!< Number: 10, Text: A{{argChan}} {{0}}, Function: Rot Absolut
#define ADR_Kocolor_red_absolute 10
#define KoADR_color_red_absoluteIndex(X) knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * X + 10)
#define KoADR_color_red_absolute knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * channelIndex() + 10)
//!< Number: 11, Text: A{{argChan}} {{0}}, Function: Rot Status
#define ADR_Kocolor_red_state 11
#define KoADR_color_red_stateIndex(X) knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * X + 11)
#define KoADR_color_red_state knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * channelIndex() + 11)
//!< Number: 12, Text: A{{argChan}} {{0}}, Function: Grün Relativ
#define ADR_Kocolor_green_relative 12
#define KoADR_color_green_relativeIndex(X) knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * X + 12)
#define KoADR_color_green_relative knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * channelIndex() + 12)
//!< Number: 13, Text: A{{argChan}} {{0}}, Function: Grün Absolut
#define ADR_Kocolor_green_absolute 13
#define KoADR_color_green_absoluteIndex(X) knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * X + 13)
#define KoADR_color_green_absolute knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * channelIndex() + 13)
//!< Number: 14, Text: A{{argChan}} {{0}}, Function: Grün Status
#define ADR_Kocolor_green_state 14
#define KoADR_color_green_stateIndex(X) knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * X + 14)
#define KoADR_color_green_state knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * channelIndex() + 14)
//!< Number: 15, Text: A{{argChan}} {{0}}, Function: Blau Relativ
#define ADR_Kocolor_blue_relative 15
#define KoADR_color_blue_relativeIndex(X) knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * X + 15)
#define KoADR_color_blue_relative knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * channelIndex() + 15)
//!< Number: 16, Text: A{{argChan}} {{0}}, Function: Blau Absolut
#define ADR_Kocolor_blue_absolute 16
#define KoADR_color_blue_absoluteIndex(X) knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * X + 16)
#define KoADR_color_blue_absolute knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * channelIndex() + 16)
//!< Number: 17, Text: A{{argChan}} {{0}}, Function: Blau Status
#define ADR_Kocolor_blue_state 17
#define KoADR_color_blue_stateIndex(X) knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * X + 17)
#define KoADR_color_blue_state knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * channelIndex() + 17)

//-----Module: group
#define GRP_deviceType		0x0000
#define GRP_deviceType_Shift	6
#define GRP_deviceType_Mask	0x0003
// Offset: 0, Size: 2 Bit, Text: Gerätetyp
#define ParamGRP_deviceTypeIndex(X) ((uint)((knx.paramByte((GRP_ParamBlockOffset + GRP_ParamBlockSize * X + 0)) >> GRP_deviceType_Shift) & GRP_deviceType_Mask))
// Offset: 0, Size: 2 Bit, Text: Gerätetyp
#define ParamGRP_deviceType ((uint)((knx.paramByte((GRP_ParamBlockOffset + GRP_ParamBlockSize * channelIndex() + 0)) >> GRP_deviceType_Shift) & GRP_deviceType_Mask))
#define GRP_type		0x0000
// Offset: 0, BitOffset: 2, Size: 1 Bit, Text: Betriebsart
#define ParamGRP_typeIndex(X) knx.paramBit((GRP_ParamBlockOffset + GRP_ParamBlockSize * X + 0), 2)
// Offset: 0, BitOffset: 2, Size: 1 Bit, Text: Betriebsart
#define ParamGRP_type knx.paramBit((GRP_ParamBlockOffset + GRP_ParamBlockSize * channelIndex() + 0), 2)
#define GRP_nachtriggern		0x0000
// Offset: 0, BitOffset: 3, Size: 1 Bit, Text: Nachtriggern erlauben
#define ParamGRP_nachtriggernIndex(X) knx.paramBit((GRP_ParamBlockOffset + GRP_ParamBlockSize * X + 0), 3)
// Offset: 0, BitOffset: 3, Size: 1 Bit, Text: Nachtriggern erlauben
#define ParamGRP_nachtriggern knx.paramBit((GRP_ParamBlockOffset + GRP_ParamBlockSize * channelIndex() + 0), 3)
#define GRP_stairtime		0x0001
#define GRP_stairtime_Shift	5
#define GRP_stairtime_Mask	0x07FF
// Offset: 1, Size: 11 Bit, Text: Nachlaufzeit
#define ParamGRP_stairtimeIndex(X) ((uint)((knx.paramWord((GRP_ParamBlockOffset + GRP_ParamBlockSize * X + 1)) >> GRP_stairtime_Shift) & GRP_stairtime_Mask))
// Offset: 1, Size: 11 Bit, Text: Nachlaufzeit
#define ParamGRP_stairtime ((uint)((knx.paramWord((GRP_ParamBlockOffset + GRP_ParamBlockSize * channelIndex() + 1)) >> GRP_stairtime_Shift) & GRP_stairtime_Mask))
#define GRP_manuoff		0x0000
// Offset: 0, BitOffset: 4, Size: 1 Bit, Text: Manuelles ausschalten
#define ParamGRP_manuoffIndex(X) knx.paramBit((GRP_ParamBlockOffset + GRP_ParamBlockSize * X + 0), 4)
// Offset: 0, BitOffset: 4, Size: 1 Bit, Text: Manuelles ausschalten
#define ParamGRP_manuoff knx.paramBit((GRP_ParamBlockOffset + GRP_ParamBlockSize * channelIndex() + 0), 4)
#define GRP_lockbehave		0x0000
#define GRP_lockbehave_Shift	1
#define GRP_lockbehave_Mask	0x0003
// Offset: 0, BitOffset: 5, Size: 2 Bit, Text: Verhalten bei Sperre
#define ParamGRP_lockbehaveIndex(X) ((uint)((knx.paramByte((GRP_ParamBlockOffset + GRP_ParamBlockSize * X + 0)) >> GRP_lockbehave_Shift) & GRP_lockbehave_Mask))
// Offset: 0, BitOffset: 5, Size: 2 Bit, Text: Verhalten bei Sperre
#define ParamGRP_lockbehave ((uint)((knx.paramByte((GRP_ParamBlockOffset + GRP_ParamBlockSize * channelIndex() + 0)) >> GRP_lockbehave_Shift) & GRP_lockbehave_Mask))
#define GRP_lockvalue		0x0003
#define GRP_lockvalue_Shift	1
#define GRP_lockvalue_Mask	0x007F
// Offset: 3, Size: 7 Bit, Text: 
#define ParamGRP_lockvalueIndex(X) ((uint)((knx.paramByte((GRP_ParamBlockOffset + GRP_ParamBlockSize * X + 3)) >> GRP_lockvalue_Shift) & GRP_lockvalue_Mask))
// Offset: 3, Size: 7 Bit, Text: 
#define ParamGRP_lockvalue ((uint)((knx.paramByte((GRP_ParamBlockOffset + GRP_ParamBlockSize * channelIndex() + 3)) >> GRP_lockvalue_Shift) & GRP_lockvalue_Mask))
#define GRP_unlockbehave		0x0004
#define GRP_unlockbehave_Shift	6
#define GRP_unlockbehave_Mask	0x0003
// Offset: 4, Size: 2 Bit, Text: Verhalten bei Entsperren
#define ParamGRP_unlockbehaveIndex(X) ((uint)((knx.paramByte((GRP_ParamBlockOffset + GRP_ParamBlockSize * X + 4)) >> GRP_unlockbehave_Shift) & GRP_unlockbehave_Mask))
// Offset: 4, Size: 2 Bit, Text: Verhalten bei Entsperren
#define ParamGRP_unlockbehave ((uint)((knx.paramByte((GRP_ParamBlockOffset + GRP_ParamBlockSize * channelIndex() + 4)) >> GRP_unlockbehave_Shift) & GRP_unlockbehave_Mask))
#define GRP_unlockvalue		0x0005
#define GRP_unlockvalue_Shift	1
#define GRP_unlockvalue_Mask	0x007F
// Offset: 5, Size: 7 Bit, Text: 
#define ParamGRP_unlockvalueIndex(X) ((uint)((knx.paramByte((GRP_ParamBlockOffset + GRP_ParamBlockSize * X + 5)) >> GRP_unlockvalue_Shift) & GRP_unlockvalue_Mask))
// Offset: 5, Size: 7 Bit, Text: 
#define ParamGRP_unlockvalue ((uint)((knx.paramByte((GRP_ParamBlockOffset + GRP_ParamBlockSize * channelIndex() + 5)) >> GRP_unlockvalue_Shift) & GRP_unlockvalue_Mask))
#define GRP_onDay		0x0006
#define GRP_onDay_Shift	1
#define GRP_onDay_Mask	0x007F
// Offset: 6, Size: 7 Bit, Text: Einschaltwert Tag
#define ParamGRP_onDayIndex(X) ((uint)((knx.paramByte((GRP_ParamBlockOffset + GRP_ParamBlockSize * X + 6)) >> GRP_onDay_Shift) & GRP_onDay_Mask))
// Offset: 6, Size: 7 Bit, Text: Einschaltwert Tag
#define ParamGRP_onDay ((uint)((knx.paramByte((GRP_ParamBlockOffset + GRP_ParamBlockSize * channelIndex() + 6)) >> GRP_onDay_Shift) & GRP_onDay_Mask))
#define GRP_onNight		0x0007
#define GRP_onNight_Shift	1
#define GRP_onNight_Mask	0x007F
// Offset: 7, Size: 7 Bit, Text: Einschaltwert Nacht
#define ParamGRP_onNightIndex(X) ((uint)((knx.paramByte((GRP_ParamBlockOffset + GRP_ParamBlockSize * X + 7)) >> GRP_onNight_Shift) & GRP_onNight_Mask))
// Offset: 7, Size: 7 Bit, Text: Einschaltwert Nacht
#define ParamGRP_onNight ((uint)((knx.paramByte((GRP_ParamBlockOffset + GRP_ParamBlockSize * channelIndex() + 7)) >> GRP_onNight_Shift) & GRP_onNight_Mask))
#define GRP_locknegate		0x0000
// Offset: 0, BitOffset: 7, Size: 1 Bit, Text: Sperren bei
#define ParamGRP_locknegateIndex(X) knx.paramBit((GRP_ParamBlockOffset + GRP_ParamBlockSize * X + 0), 7)
// Offset: 0, BitOffset: 7, Size: 1 Bit, Text: Sperren bei
#define ParamGRP_locknegate knx.paramBit((GRP_ParamBlockOffset + GRP_ParamBlockSize * channelIndex() + 0), 7)
//!< Number: 0, Text: G{{argChan}} {{0}}, Function: Schalten
#define GRP_Koswitch 0
#define KoGRP_switchIndex(X) knx.getGroupObject(GRP_KoOffset + GRP_KoBlockSize * X + 0)
#define KoGRP_switch knx.getGroupObject(GRP_KoOffset + GRP_KoBlockSize * channelIndex() + 0)
//!< Number: 1, Text: G{{argChan}} {{0}}, Function: Schalten Status
#define GRP_Koswitch_state 1
#define KoGRP_switch_stateIndex(X) knx.getGroupObject(GRP_KoOffset + GRP_KoBlockSize * X + 1)
#define KoGRP_switch_state knx.getGroupObject(GRP_KoOffset + GRP_KoBlockSize * channelIndex() + 1)
//!< Number: 2, Text: G{{argChan}} {{0}}, Function: Dimmen Relativ
#define GRP_Kodimm_relative 2
#define KoGRP_dimm_relativeIndex(X) knx.getGroupObject(GRP_KoOffset + GRP_KoBlockSize * X + 2)
#define KoGRP_dimm_relative knx.getGroupObject(GRP_KoOffset + GRP_KoBlockSize * channelIndex() + 2)
//!< Number: 3, Text: G{{argChan}} {{0}}, Function: Dimmen Absolut
#define GRP_Kodimm_absolute 3
#define KoGRP_dimm_absoluteIndex(X) knx.getGroupObject(GRP_KoOffset + GRP_KoBlockSize * X + 3)
#define KoGRP_dimm_absolute knx.getGroupObject(GRP_KoOffset + GRP_KoBlockSize * channelIndex() + 3)
//!< Number: 4, Text: G{{argChan}} {{0}}, Function: Dimmen Status
#define GRP_Kodimm_state 4
#define KoGRP_dimm_stateIndex(X) knx.getGroupObject(GRP_KoOffset + GRP_KoBlockSize * X + 4)
#define KoGRP_dimm_state knx.getGroupObject(GRP_KoOffset + GRP_KoBlockSize * channelIndex() + 4)
//!< Number: 5, Text: G{{argChan}} {{0}}, Function: Sperren
#define GRP_Kolock 5
#define KoGRP_lockIndex(X) knx.getGroupObject(GRP_KoOffset + GRP_KoBlockSize * X + 5)
#define KoGRP_lock knx.getGroupObject(GRP_KoOffset + GRP_KoBlockSize * channelIndex() + 5)

//-----Module: scene
#define SCE_type		0x0000
#define SCE_type_Shift	6
#define SCE_type_Mask	0x0003
// Offset: 0, Size: 2 Bit, Text: Typ
#define ParamSCE_typeIndex(X) ((uint)((knx.paramByte((SCE_ParamBlockOffset + SCE_ParamBlockSize * X + 0)) >> SCE_type_Shift) & SCE_type_Mask))
// Offset: 0, Size: 2 Bit, Text: Typ
#define ParamSCE_type ((uint)((knx.paramByte((SCE_ParamBlockOffset + SCE_ParamBlockSize * channelIndex() + 0)) >> SCE_type_Shift) & SCE_type_Mask))
#define SCE_save		0x0000
// Offset: 0, BitOffset: 2, Size: 1 Bit, Text: Speichern erlauben
#define ParamSCE_saveIndex(X) knx.paramBit((SCE_ParamBlockOffset + SCE_ParamBlockSize * X + 0), 2)
// Offset: 0, BitOffset: 2, Size: 1 Bit, Text: Speichern erlauben
#define ParamSCE_save knx.paramBit((SCE_ParamBlockOffset + SCE_ParamBlockSize * channelIndex() + 0), 2)
#define SCE_numberKnx		0x0001
#define SCE_numberKnx_Shift	1
#define SCE_numberKnx_Mask	0x007F
// Offset: 1, Size: 7 Bit, Text: Szenennummer Knx
#define ParamSCE_numberKnxIndex(X) ((uint)((knx.paramByte((SCE_ParamBlockOffset + SCE_ParamBlockSize * X + 1)) >> SCE_numberKnx_Shift) & SCE_numberKnx_Mask))
// Offset: 1, Size: 7 Bit, Text: Szenennummer Knx
#define ParamSCE_numberKnx ((uint)((knx.paramByte((SCE_ParamBlockOffset + SCE_ParamBlockSize * channelIndex() + 1)) >> SCE_numberKnx_Shift) & SCE_numberKnx_Mask))
#define SCE_numberDali		0x0000
#define SCE_numberDali_Shift	1
#define SCE_numberDali_Mask	0x000F
// Offset: 0, BitOffset: 3, Size: 4 Bit, Text: Szenennummer Dali
#define ParamSCE_numberDaliIndex(X) ((uint)((knx.paramByte((SCE_ParamBlockOffset + SCE_ParamBlockSize * X + 0)) >> SCE_numberDali_Shift) & SCE_numberDali_Mask))
// Offset: 0, BitOffset: 3, Size: 4 Bit, Text: Szenennummer Dali
#define ParamSCE_numberDali ((uint)((knx.paramByte((SCE_ParamBlockOffset + SCE_ParamBlockSize * channelIndex() + 0)) >> SCE_numberDali_Shift) & SCE_numberDali_Mask))
#define SCE_address		0x0002
#define SCE_address_Shift	2
#define SCE_address_Mask	0x003F
// Offset: 2, Size: 6 Bit, Text: Dali Adresse
#define ParamSCE_addressIndex(X) ((uint)((knx.paramByte((SCE_ParamBlockOffset + SCE_ParamBlockSize * X + 2)) >> SCE_address_Shift) & SCE_address_Mask))
// Offset: 2, Size: 6 Bit, Text: Dali Adresse
#define ParamSCE_address ((uint)((knx.paramByte((SCE_ParamBlockOffset + SCE_ParamBlockSize * channelIndex() + 2)) >> SCE_address_Shift) & SCE_address_Mask))
#define SCE_group		0x0003
#define SCE_group_Shift	4
#define SCE_group_Mask	0x000F
// Offset: 3, Size: 4 Bit, Text: Dali Gruppe
#define ParamSCE_groupIndex(X) ((uint)((knx.paramByte((SCE_ParamBlockOffset + SCE_ParamBlockSize * X + 3)) >> SCE_group_Shift) & SCE_group_Mask))
// Offset: 3, Size: 4 Bit, Text: Dali Gruppe
#define ParamSCE_group ((uint)((knx.paramByte((SCE_ParamBlockOffset + SCE_ParamBlockSize * channelIndex() + 3)) >> SCE_group_Shift) & SCE_group_Mask))


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
#define MAIN_ApplicationVersion 0x17
#define MAIN_OrderNumber "TW-DALI.GW.01" //may not work with multiple devices on same hardware or app on different hardware
#define MAIN_ParameterSize 753
#define MAIN_MaxKoNumber 629


#define APP_daynight		0x0000
// Offset: 0, Size: 1 Bit, Text: Tag/Nacht Objekt
#define ParamAPP_daynight knx.paramBit(0, 0)
#define APP_scenesave		0x0000
// Offset: 0, BitOffset: 1, Size: 1 Bit, Text: Szene speichern erlauben
#define ParamAPP_scenesave knx.paramBit(0, 1)
#define APP_szeneoffset		0x0000
#define APP_szeneoffset_Mask	0x003F
// Offset: 0, BitOffset: 2, Size: 6 Bit, Text: Szenennummer offset
#define ParamAPP_szeneoffset ((uint)((knx.paramByte(0)) & APP_szeneoffset_Mask))
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
#define ADR_ParamBlockOffset 1
#define ADR_ParamBlockSize 10
#define GRP_ParamBlockOffset 641
#define GRP_ParamBlockSize 7
#define ADR_KoOffset 6
#define ADR_KoBlockSize 8
#define GRP_KoOffset 518
#define GRP_KoBlockSize 7

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
#define ADR_scenesave		0x0002
// Offset: 2, BitOffset: 7, Size: 1 Bit, Text: Szene speichern erlauben
#define ParamADR_scenesaveIndex(X) knx.paramBit((ADR_ParamBlockOffset + ADR_ParamBlockSize * X + 2), 7)
// Offset: 2, BitOffset: 7, Size: 1 Bit, Text: Szene speichern erlauben
#define ParamADR_scenesave knx.paramBit((ADR_ParamBlockOffset + ADR_ParamBlockSize * channelIndex() + 2), 7)
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
//!< Number: 7, Text: A{{argChan}} {{0}}, Function: Szene
#define ADR_Koscene 7
#define KoADR_sceneIndex(X) knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * X + 7)
#define KoADR_scene knx.getGroupObject(ADR_KoOffset + ADR_KoBlockSize * channelIndex() + 7)

//-----Module: group
#define GRP_type		0x0000
// Offset: 0, Size: 1 Bit, Text: Betriebsart
#define ParamGRP_typeIndex(X) knx.paramBit((GRP_ParamBlockOffset + GRP_ParamBlockSize * X + 0), 0)
// Offset: 0, Size: 1 Bit, Text: Betriebsart
#define ParamGRP_type knx.paramBit((GRP_ParamBlockOffset + GRP_ParamBlockSize * channelIndex() + 0), 0)
#define GRP_nachtriggern		0x0000
// Offset: 0, BitOffset: 1, Size: 1 Bit, Text: Nachtriggern erlauben
#define ParamGRP_nachtriggernIndex(X) knx.paramBit((GRP_ParamBlockOffset + GRP_ParamBlockSize * X + 0), 1)
// Offset: 0, BitOffset: 1, Size: 1 Bit, Text: Nachtriggern erlauben
#define ParamGRP_nachtriggern knx.paramBit((GRP_ParamBlockOffset + GRP_ParamBlockSize * channelIndex() + 0), 1)
#define GRP_stairtime		0x0001
#define GRP_stairtime_Shift	5
#define GRP_stairtime_Mask	0x07FF
// Offset: 1, Size: 11 Bit, Text: Nachlaufzeit
#define ParamGRP_stairtimeIndex(X) ((uint)((knx.paramWord((GRP_ParamBlockOffset + GRP_ParamBlockSize * X + 1)) >> GRP_stairtime_Shift) & GRP_stairtime_Mask))
// Offset: 1, Size: 11 Bit, Text: Nachlaufzeit
#define ParamGRP_stairtime ((uint)((knx.paramWord((GRP_ParamBlockOffset + GRP_ParamBlockSize * channelIndex() + 1)) >> GRP_stairtime_Shift) & GRP_stairtime_Mask))
#define GRP_manuoff		0x0000
// Offset: 0, BitOffset: 2, Size: 1 Bit, Text: Manuelles ausschalten
#define ParamGRP_manuoffIndex(X) knx.paramBit((GRP_ParamBlockOffset + GRP_ParamBlockSize * X + 0), 2)
// Offset: 0, BitOffset: 2, Size: 1 Bit, Text: Manuelles ausschalten
#define ParamGRP_manuoff knx.paramBit((GRP_ParamBlockOffset + GRP_ParamBlockSize * channelIndex() + 0), 2)
#define GRP_lockbehave		0x0000
#define GRP_lockbehave_Shift	3
#define GRP_lockbehave_Mask	0x0003
// Offset: 0, BitOffset: 3, Size: 2 Bit, Text: Verhalten bei Sperre
#define ParamGRP_lockbehaveIndex(X) ((uint)((knx.paramByte((GRP_ParamBlockOffset + GRP_ParamBlockSize * X + 0)) >> GRP_lockbehave_Shift) & GRP_lockbehave_Mask))
// Offset: 0, BitOffset: 3, Size: 2 Bit, Text: Verhalten bei Sperre
#define ParamGRP_lockbehave ((uint)((knx.paramByte((GRP_ParamBlockOffset + GRP_ParamBlockSize * channelIndex() + 0)) >> GRP_lockbehave_Shift) & GRP_lockbehave_Mask))
#define GRP_lockvalue		0x0003
#define GRP_lockvalue_Shift	1
#define GRP_lockvalue_Mask	0x007F
// Offset: 3, Size: 7 Bit, Text: 
#define ParamGRP_lockvalueIndex(X) ((uint)((knx.paramByte((GRP_ParamBlockOffset + GRP_ParamBlockSize * X + 3)) >> GRP_lockvalue_Shift) & GRP_lockvalue_Mask))
// Offset: 3, Size: 7 Bit, Text: 
#define ParamGRP_lockvalue ((uint)((knx.paramByte((GRP_ParamBlockOffset + GRP_ParamBlockSize * channelIndex() + 3)) >> GRP_lockvalue_Shift) & GRP_lockvalue_Mask))
#define GRP_unlockbehave		0x0000
#define GRP_unlockbehave_Shift	1
#define GRP_unlockbehave_Mask	0x0003
// Offset: 0, BitOffset: 5, Size: 2 Bit, Text: Verhalten bei Entsperren
#define ParamGRP_unlockbehaveIndex(X) ((uint)((knx.paramByte((GRP_ParamBlockOffset + GRP_ParamBlockSize * X + 0)) >> GRP_unlockbehave_Shift) & GRP_unlockbehave_Mask))
// Offset: 0, BitOffset: 5, Size: 2 Bit, Text: Verhalten bei Entsperren
#define ParamGRP_unlockbehave ((uint)((knx.paramByte((GRP_ParamBlockOffset + GRP_ParamBlockSize * channelIndex() + 0)) >> GRP_unlockbehave_Shift) & GRP_unlockbehave_Mask))
#define GRP_unlockvalue		0x0004
#define GRP_unlockvalue_Shift	1
#define GRP_unlockvalue_Mask	0x007F
// Offset: 4, Size: 7 Bit, Text: 
#define ParamGRP_unlockvalueIndex(X) ((uint)((knx.paramByte((GRP_ParamBlockOffset + GRP_ParamBlockSize * X + 4)) >> GRP_unlockvalue_Shift) & GRP_unlockvalue_Mask))
// Offset: 4, Size: 7 Bit, Text: 
#define ParamGRP_unlockvalue ((uint)((knx.paramByte((GRP_ParamBlockOffset + GRP_ParamBlockSize * channelIndex() + 4)) >> GRP_unlockvalue_Shift) & GRP_unlockvalue_Mask))
#define GRP_onDay		0x0005
#define GRP_onDay_Shift	1
#define GRP_onDay_Mask	0x007F
// Offset: 5, Size: 7 Bit, Text: Einschaltwert Tag
#define ParamGRP_onDayIndex(X) ((uint)((knx.paramByte((GRP_ParamBlockOffset + GRP_ParamBlockSize * X + 5)) >> GRP_onDay_Shift) & GRP_onDay_Mask))
// Offset: 5, Size: 7 Bit, Text: Einschaltwert Tag
#define ParamGRP_onDay ((uint)((knx.paramByte((GRP_ParamBlockOffset + GRP_ParamBlockSize * channelIndex() + 5)) >> GRP_onDay_Shift) & GRP_onDay_Mask))
#define GRP_onNight		0x0006
#define GRP_onNight_Shift	1
#define GRP_onNight_Mask	0x007F
// Offset: 6, Size: 7 Bit, Text: Einschaltwert Nacht
#define ParamGRP_onNightIndex(X) ((uint)((knx.paramByte((GRP_ParamBlockOffset + GRP_ParamBlockSize * X + 6)) >> GRP_onNight_Shift) & GRP_onNight_Mask))
// Offset: 6, Size: 7 Bit, Text: Einschaltwert Nacht
#define ParamGRP_onNight ((uint)((knx.paramByte((GRP_ParamBlockOffset + GRP_ParamBlockSize * channelIndex() + 6)) >> GRP_onNight_Shift) & GRP_onNight_Mask))
#define GRP_locknegate		0x0000
// Offset: 0, BitOffset: 7, Size: 1 Bit, Text: Sperren bei
#define ParamGRP_locknegateIndex(X) knx.paramBit((GRP_ParamBlockOffset + GRP_ParamBlockSize * X + 0), 7)
// Offset: 0, BitOffset: 7, Size: 1 Bit, Text: Sperren bei
#define ParamGRP_locknegate knx.paramBit((GRP_ParamBlockOffset + GRP_ParamBlockSize * channelIndex() + 0), 7)
#define GRP_scenesave		0x0003
// Offset: 3, BitOffset: 7, Size: 1 Bit, Text: Szene speichern erlauben
#define ParamGRP_scenesaveIndex(X) knx.paramBit((GRP_ParamBlockOffset + GRP_ParamBlockSize * X + 3), 7)
// Offset: 3, BitOffset: 7, Size: 1 Bit, Text: Szene speichern erlauben
#define ParamGRP_scenesave knx.paramBit((GRP_ParamBlockOffset + GRP_ParamBlockSize * channelIndex() + 3), 7)
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
//!< Number: 6, Text: G{{argChan}} {{0}}, Function: Szene
#define GRP_Koscene 6
#define KoGRP_sceneIndex(X) knx.getGroupObject(GRP_KoOffset + GRP_KoBlockSize * X + 6)
#define KoGRP_scene knx.getGroupObject(GRP_KoOffset + GRP_KoBlockSize * channelIndex() + 6)


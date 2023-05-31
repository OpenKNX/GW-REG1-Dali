#pragma once

#define paramDelay(time) (uint32_t)( \
            (time & 0xC000) == 0xC000 ? (time & 0x3FFF) * 100 : \
            (time & 0xC000) == 0x0000 ? (time & 0x3FFF) * 1000 : \
            (time & 0xC000) == 0x4000 ? (time & 0x3FFF) * 60000 : \
            (time & 0xC000) == 0x8000 ? ((time & 0x3FFF) > 1000 ? 3600000 : \
                                         (time & 0x3FFF) * 3600000 ) : 0 )
//--------------------Allgemein---------------------------
#define MAIN_OpenKnxId 0xA4
#define MAIN_ApplicationNumber 0x00
#define MAIN_ApplicationVersion 0x11
#define MAIN_OrderNumber "TWB-IR.01" //may not work with multiple devices on same hardware or app on different hardware
#define MAIN_ParameterSize 80
#define MAIN_MaxKoNumber 47



//---------------------Modules----------------------------

//-----Module specific starts
#define IR_ParamBlockOffset 0
#define IR_ParamBlockSize 5
#define IR_KoOffset 0
#define IR_KoBlockSize 2

//-----Module: ir
#define IR_inOutType		0x0000
#define IR_inOutType_Shift	6
#define IR_inOutType_Mask	0x0003
// Offset: 0, Size: 2 Bit, Text: Kanal
#define ParamIR_inOutTypeIndex(X) ((uint)((knx.paramByte((IR_ParamBlockOffset + IR_ParamBlockSize * X + 0)) >> IR_inOutType_Shift) & IR_inOutType_Mask))
// Offset: 0, Size: 2 Bit, Text: Kanal
#define ParamIR_inOutType ((uint)((knx.paramByte((IR_ParamBlockOffset + IR_ParamBlockSize * channelIndex() + 0)) >> IR_inOutType_Shift) & IR_inOutType_Mask))
#define IR_inType		0x0004
#define IR_inType_Shift	5
#define IR_inType_Mask	0x0007
// UnionOffset: 1, ParaOffset: 3, Size: 3 Bit, Text: Funktion
#define ParamIR_inTypeIndex(X) ((uint)((knx.paramByte((IR_ParamBlockOffset + IR_ParamBlockSize * X + 4)) >> IR_inType_Shift) & IR_inType_Mask))
// UnionOffset: 1, ParaOffset: 3, Size: 3 Bit, Text: Funktion
#define ParamIR_inType ((uint)((knx.paramByte((IR_ParamBlockOffset + IR_ParamBlockSize * channelIndex() + 4)) >> IR_inType_Shift) & IR_inType_Mask))
#define IR_inSwitch		0x0001
#define IR_inSwitch_Shift	6
#define IR_inSwitch_Mask	0x0003
// UnionOffset: 1, ParaOffset: 0, Size: 2 Bit, Text: Unterfunktion
#define ParamIR_inSwitchIndex(X) ((uint)((knx.paramByte((IR_ParamBlockOffset + IR_ParamBlockSize * X + 1)) >> IR_inSwitch_Shift) & IR_inSwitch_Mask))
// UnionOffset: 1, ParaOffset: 0, Size: 2 Bit, Text: Unterfunktion
#define ParamIR_inSwitch ((uint)((knx.paramByte((IR_ParamBlockOffset + IR_ParamBlockSize * channelIndex() + 1)) >> IR_inSwitch_Shift) & IR_inSwitch_Mask))
#define IR_inValue		0x0002
// UnionOffset: 1, ParaOffset: 1, Size: 8 Bit (1 Byte), Text: Wert
#define ParamIR_inValueIndex(X) ((uint)((knx.paramByte((IR_ParamBlockOffset + IR_ParamBlockSize * X + 2)))))
// UnionOffset: 1, ParaOffset: 1, Size: 8 Bit (1 Byte), Text: Wert
#define ParamIR_inValue ((uint)((knx.paramByte((IR_ParamBlockOffset + IR_ParamBlockSize * channelIndex() + 2)))))
#define IR_inScene		0x0002
#define IR_inScene_Shift	2
#define IR_inScene_Mask	0x003F
// UnionOffset: 1, ParaOffset: 1, Size: 6 Bit, Text: Szenennummer
#define ParamIR_inSceneIndex(X) ((uint)((knx.paramByte((IR_ParamBlockOffset + IR_ParamBlockSize * X + 2)) >> IR_inScene_Shift) & IR_inScene_Mask))
// UnionOffset: 1, ParaOffset: 1, Size: 6 Bit, Text: Szenennummer
#define ParamIR_inScene ((uint)((knx.paramByte((IR_ParamBlockOffset + IR_ParamBlockSize * channelIndex() + 2)) >> IR_inScene_Shift) & IR_inScene_Mask))
#define IR_inDimmDirection		0x0001
// UnionOffset: 1, ParaOffset: 0, Size: 1 Bit, Text: Richtung
#define ParamIR_inDimmDirectionIndex(X) knx.paramBit((IR_ParamBlockOffset + IR_ParamBlockSize * X + 1), 0)
// UnionOffset: 1, ParaOffset: 0, Size: 1 Bit, Text: Richtung
#define ParamIR_inDimmDirection knx.paramBit((IR_ParamBlockOffset + IR_ParamBlockSize * channelIndex() + 1), 0)
#define IR_inDimmSwitch		0x0001
// UnionOffset: 1, ParaOffset: 0, BitOffset: 1, Size: 1 Bit, Text: Ein/Aus bei einmaligem Tastendruck senden
#define ParamIR_inDimmSwitchIndex(X) knx.paramBit((IR_ParamBlockOffset + IR_ParamBlockSize * X + 1), 1)
// UnionOffset: 1, ParaOffset: 0, BitOffset: 1, Size: 1 Bit, Text: Ein/Aus bei einmaligem Tastendruck senden
#define ParamIR_inDimmSwitch knx.paramBit((IR_ParamBlockOffset + IR_ParamBlockSize * channelIndex() + 1), 1)
#define IR_inColor		0x0001
// UnionOffset: 1, ParaOffset: 0, Size: 24 Bit (3 Byte), Text: Farbe
#define ParamIR_inColorIndex(X) knx.paramData((IR_ParamBlockOffset + IR_ParamBlockSize * X + 1))
// UnionOffset: 1, ParaOffset: 0, Size: 24 Bit (3 Byte), Text: Farbe
#define ParamIR_inColor knx.paramData((IR_ParamBlockOffset + IR_ParamBlockSize * channelIndex() + 1))
#define IR_inColorType		0x0004
// UnionOffset: 1, ParaOffset: 3, BitOffset: 2, Size: 1 Bit, Text: Ausgabe als
#define ParamIR_inColorTypeIndex(X) knx.paramBit((IR_ParamBlockOffset + IR_ParamBlockSize * X + 4), 2)
// UnionOffset: 1, ParaOffset: 3, BitOffset: 2, Size: 1 Bit, Text: Ausgabe als
#define ParamIR_inColorType knx.paramBit((IR_ParamBlockOffset + IR_ParamBlockSize * channelIndex() + 4), 2)
#define IR_outType		0x0004
#define IR_outType_Shift	6
#define IR_outType_Mask	0x0003
// UnionOffset: 1, ParaOffset: 3, Size: 2 Bit, Text: Funktion
#define ParamIR_outTypeIndex(X) ((uint)((knx.paramByte((IR_ParamBlockOffset + IR_ParamBlockSize * X + 4)) >> IR_outType_Shift) & IR_outType_Mask))
// UnionOffset: 1, ParaOffset: 3, Size: 2 Bit, Text: Funktion
#define ParamIR_outType ((uint)((knx.paramByte((IR_ParamBlockOffset + IR_ParamBlockSize * channelIndex() + 4)) >> IR_outType_Shift) & IR_outType_Mask))
#define IR_outValue		0x0002
// UnionOffset: 1, ParaOffset: 1, Size: 8 Bit (1 Byte), Text: Wert
#define ParamIR_outValueIndex(X) ((uint)((knx.paramByte((IR_ParamBlockOffset + IR_ParamBlockSize * X + 2)))))
// UnionOffset: 1, ParaOffset: 1, Size: 8 Bit (1 Byte), Text: Wert
#define ParamIR_outValue ((uint)((knx.paramByte((IR_ParamBlockOffset + IR_ParamBlockSize * channelIndex() + 2)))))
#define IR_outScene		0x0002
#define IR_outScene_Shift	2
#define IR_outScene_Mask	0x003F
// UnionOffset: 1, ParaOffset: 1, Size: 6 Bit, Text: Szenennummer
#define ParamIR_outSceneIndex(X) ((uint)((knx.paramByte((IR_ParamBlockOffset + IR_ParamBlockSize * X + 2)) >> IR_outScene_Shift) & IR_outScene_Mask))
// UnionOffset: 1, ParaOffset: 1, Size: 6 Bit, Text: Szenennummer
#define ParamIR_outScene ((uint)((knx.paramByte((IR_ParamBlockOffset + IR_ParamBlockSize * channelIndex() + 2)) >> IR_outScene_Shift) & IR_outScene_Mask))
#define IR_outSwitch		0x0001
#define IR_outSwitch_Shift	6
#define IR_outSwitch_Mask	0x0003
// UnionOffset: 1, ParaOffset: 0, Size: 2 Bit, Text: Senden bei
#define ParamIR_outSwitchIndex(X) ((uint)((knx.paramByte((IR_ParamBlockOffset + IR_ParamBlockSize * X + 1)) >> IR_outSwitch_Shift) & IR_outSwitch_Mask))
// UnionOffset: 1, ParaOffset: 0, Size: 2 Bit, Text: Senden bei
#define ParamIR_outSwitch ((uint)((knx.paramByte((IR_ParamBlockOffset + IR_ParamBlockSize * channelIndex() + 1)) >> IR_outSwitch_Shift) & IR_outSwitch_Mask))
//!< Number: 1, Text: IR{{argChan}} <- {{0:}}, Function: Farbe
#define IR_Koco_n1 1
#define KoIR_co_n1Index(X) knx.getGroupObject(IR_KoOffset + IR_KoBlockSize * X + 1)
#define KoIR_co_n1 knx.getGroupObject(IR_KoOffset + IR_KoBlockSize * channelIndex() + 1)
//!< Number: 2, Text: IR{{argChan}} <- {{0:}}, Function: Status
#define IR_Koco_n2 2
#define KoIR_co_n2Index(X) knx.getGroupObject(IR_KoOffset + IR_KoBlockSize * X + 2)
#define KoIR_co_n2 knx.getGroupObject(IR_KoOffset + IR_KoBlockSize * channelIndex() + 2)


/*

              <ParameterType Id="%AID%_PT-deviceType" Name="deviceType">
                <TypeRestriction Base="Value" SizeInBit="4">
                  <Enumeration Text="Deaktiviert" Value="0" Id="%ENID%" />
                  <Enumeration Text="DT0 - Fluoreszierend" Value="1" Id="%ENID%" />
                  <Enumeration Text="DT1 - Eigenständige Notallbeleuchtung" Value="2" Id="%ENID%" />
                  <Enumeration Text="DT2 - Entladungslampe" Value="3" Id="%ENID%" />
                  <Enumeration Text="DT3 - Niedervolt Halogen" Value="4" Id="%ENID%" />
                  <Enumeration Text="DT4 - Glühlampe" Value="5" Id="%ENID%" />
                  <Enumeration Text="DT5 - DC Converter (1-10V, 0-10V)" Value="6" Id="%ENID%" />
                  <Enumeration Text="DT6 - LED" Value="7" Id="%ENID%" />
                  <Enumeration Text="DT7 - Schaltrelais" Value="8" Id="%ENID%" />
                  <Enumeration Text="DT8 - Farbe" Value="9" Id="%ENID%" />
                </TypeRestriction>
              </ParameterType>
*/

#define PT_deviceType_Deaktiviert 0
#define PT_deviceType_DT0 1
#define PT_deviceType_DT1 2
#define PT_deviceType_DT2 3
#define PT_deviceType_DT3 4
#define PT_deviceType_DT4 5
#define PT_deviceType_DT5 6
#define PT_deviceType_DT6 7
#define PT_deviceType_DT7 8
#define PT_deviceType_DT8 9

/*

              <ParameterType Id="%AID%_PT-lock" Name="lock">
                <TypeRestriction Base="Value" SizeInBit="2">
                  <Enumeration Text="Keine Änderung" Value="0" Id="%ENID%" />
                  <Enumeration Text="Anschalten" Value="1" Id="%ENID%" />
                  <Enumeration Text="Ausschalten" Value="2" Id="%ENID%" />
                  <Enumeration Text="Fester Wert in %" Value="3" Id="%ENID%" />
                </TypeRestriction>
              </ParameterType>
*/

#define PT_lock_no 0
#define PT_lock_on 1
#define PT_lock_off 2
#define PT_lock_value 3

/*

              <ParameterType Id="%AID%_PT-colorType" Name="colorType">
                <TypeRestriction Base="Value" SizeInBit="2">
                  <Enumeration Text="HSV" Value="0" Id="%ENID%" />
                  <Enumeration Text="RGB" Value="1" Id="%ENID%" />
                  <Enumeration Text="TW" Value="2" Id="%ENID%" />
                  <Enumeration Text="xyY / xy" Value="3" Id="%ENID%" />
                </TypeRestriction>
              </ParameterType>
*/

#define PT_colorType_HSV 0
#define PT_colorType_RGB 1
#define PT_colorType_TW 2
#define PT_colorType_XYY 3

/*

              <ParameterType Id="%AID%_PT-dimmLock" Name="dimmLock">
                <TypeRestriction Base="Value" SizeInBit="2">
                  <Enumeration Text="Nichts sperren" Value="0" Id="%AID%_PT-dimmLock_EN-0" />
                  <Enumeration Text="Kein Anschalten mit relativem Dimmen" Value="1" Id="%AID%_PT-dimmLock_EN-1" />
                  <Enumeration Text="Kein Ausschalten mit relativem Dimmen" Value="2" Id="%AID%_PT-dimmLock_EN-2" />
                  <Enumeration Text="Kein An-/Aussschalten mit relativem Dimmen" Value="3" Id="%AID%_PT-dimmLock_EN-3" />
                </TypeRestriction>
              </ParameterType>
            </ParameterTypes>
*/

#define PT_dimmLock_none 0
#define PT_dimmLock_noOn 1
#define PT_dimmLock_noOff 2
#define PT_dimmLock_noBoth 3

/*

              <ParameterType Id="%AID%_PT-colorSpace" Name="colorSpace">
                <TypeRestriction Base="Value" SizeInBit="1">
                  <Enumeration Text="RGB Werte" Value="1" Id="%ENID%" />
                  <Enumeration Text="X/Y Koordinaten" Value="0" Id="%ENID%" />
                </TypeRestriction>
              </ParameterType>
*/

#define PT_colorSpace_rgb 1
#define PT_colorSpace_xy 0

/*

              <ParameterType Id="%AID%_PT-groupType" Name="groupType">
                <TypeRestriction Base="Value" SizeInBit="2">
                  <Enumeration Text="Deaktiviert" Value="0" Id="%ENID%" />
                  <Enumeration Text="Schalten" Value="1" Id="%ENID%" />
                  <Enumeration Text="Dimmen" Value="2" Id="%ENID%" />
                  <Enumeration Text="Farbig" Value="3" Id="%ENID%" />
                </TypeRestriction>
              </ParameterType>
*/

#define PT_groupType_none 0
#define PT_groupType_switch 1
#define PT_groupType_dimm 2
#define PT_groupType_color 3

/*

              <ParameterType Id="%AID%_PT-clickAction" Name="clickAction">
                <TypeRestriction Base="Value" SizeInBit="3">
                  <Enumeration Text="Deaktiviert" Value="0" Id="%ENID%" />
                  <Enumeration Text="Alle Einschalten" Value="1" Id="%ENID%" />
                  <Enumeration Text="Alle Ausschalten" Value="2" Id="%ENID%" />
                  <Enumeration Text="Alle Umschalten" Value="3" Id="%ENID%" />
                  <Enumeration Text="Sperren" Value="4" Id="%ENID%" />
                  <Enumeration Text="Entsperren" Value="5" Id="%ENID%" />
                  <Enumeration Text="Sperre Umschalten" Value="6" Id="%ENID%" />
                  <Enumeration Text="Gerät identifizieren (LED blink - 1 = Adresse)" Value="7" Id="%ENID%" />
                </TypeRestriction>
              </ParameterType>
*/

#define PT_clickAction_none 0
#define PT_clickAction_on 1
#define PT_clickAction_off 2
#define PT_clickAction_toggle 3
#define PT_clickAction_lock 4
#define PT_clickAction_unlock 5
#define PT_clickAction_lock_toggle 6
#define PT_clickAction_identify 7

/*

              <ParameterType Id="%AID%_PT-offset" Name="offset">
                <TypeRestriction Base="Value" SizeInBit="2">
                  <Enumeration Text="Deaktiviert" Value="0" Id="%AID%_PT-offset_EN-0" />
                  <Enumeration Text="Später" Value="1" Id="%AID%_PT-offset_EN-1" />
                  <Enumeration Text="Früher" Value="2" Id="%AID%_PT-offset_EN-2" />
                </TypeRestriction>
              </ParameterType>
*/

#define PT_offset_none 0
#define PT_offset_plus 1
#define PT_offset_minus 2

/*

              <ParameterType Id="%AID%_PT-scenetype" Name="scenetype">
                <TypeRestriction Base="Value" SizeInBit="2">
                  <Enumeration Text="Deaktiviert" Value="0" Id="%ENID%" />
                  <Enumeration Text="Adresse" Value="1" Id="%ENID%" />
                  <Enumeration Text="Gruppe" Value="2" Id="%ENID%" />
                  <Enumeration Text="Broadcast" Value="3" Id="%ENID%" />
                </TypeRestriction>
              </ParameterType>
*/

#define PT_scenetype_none 0
#define PT_scenetype_address 1
#define PT_scenetype_group 2
#define PT_scenetype_broadcast 3

/*

              <ParameterType Id="%AID%_PT-hclType" Name="hclType">
                <TypeRestriction Base="Value" SizeInBit="2">
                  <Enumeration Text="Inaktiv" Value="0" Id="%AID%_PT-hclType_EN-0" />
                  <Enumeration Text="Sonnen Auf-/Untergang" Value="1" Id="%AID%_PT-hclType_EN-1" />
                  <Enumeration Text="Feste Zeiten" Value="2" Id="%AID%_PT-hclType_EN-2" />
                </TypeRestriction>
              </ParameterType>
*/

#define PT_hclType_none 0
#define PT_hclType_sun 1
#define PT_hclType_time 2
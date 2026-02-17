EESchema Schematic File Version 4
LIBS:UP1-AppTemplate-cache
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "KUPS-AppTemplate"
Date "2022-03-08"
Rev "V00.02"
Comp "OpenKNX"
Comment1 "by SirSydom <com@sirsydom.de>"
Comment2 ""
Comment3 ""
Comment4 "https://OpenKNX.de"
$EndDescr
$Comp
L OpenKNX:OpenKNX_UP1-Controller2040 J?
U 1 1 61F47946
P 8200 1850
AR Path="/606C5AC9/61F47946" Ref="J?"  Part="1" 
AR Path="/60F06D6E/61F47946" Ref="J?"  Part="1" 
AR Path="/61F47946" Ref="J1"  Part="1" 
F 0 "J1" H 8118 625 50  0000 C CNN
F 1 "OpenKNX_UP1-Controller2040" H 8118 716 50  0000 C CNN
F 2 "OpenKNX:OpenKNX_UP1-PCBConnector_1x20_App_3D" H 8200 1850 50  0001 C CNN
F 3 "~" H 8200 1850 50  0001 C CNN
F 4 "C2334" H 8200 1850 50  0001 C CNN "LCSC"
F 5 "0.5" H 8200 1850 50  0001 C CNN "LCSC Cnt"
	1    8200 1850
	-1   0    0    -1  
$EndComp
Wire Wire Line
	7750 900  7850 900 
Wire Wire Line
	7750 1000 7850 1000
Wire Wire Line
	7850 1100 7750 1100
Text GLabel 7750 1000 0    50   Input ~ 0
3V3
Text GLabel 7750 1100 0    50   Input ~ 0
5V
Text GLabel 7750 900  0    50   Input ~ 0
GND
Text Notes 8750 1350 0    50   ~ 0
GP14: SPI1 SCK / UART0 CTS / I2C1 SDA / PWM7 A
Text Notes 8750 1450 0    50   ~ 0
GP15: SPI1 TX  / UART0 RTS / I2C1 SCL / PWM7 B
Text Notes 8750 1750 0    50   ~ 0
GP18: SPI0 SCK / UART0 CTS / I2C1 SDA / PWM1 A
Text Notes 8750 1850 0    50   ~ 0
GP19: SPI0 TX  / UART0 RTS / I2C1 SCL / PWM1 B
Text Notes 8750 1650 0    50   ~ 0
GP17: SPI0 CS  / UART0 RX  / I2C0 SCL / PWM0 B
Text Notes 8750 1550 0    50   ~ 0
GP16: SPI0 RX  / UART0 TX  / I2C0 SDA / PWM0 A
Text Notes 10800 1250 2    50   ~ 0
GP13: SPI1 CS  / UART0 RX  / I2C0 SCL / PWM6 B
Text Notes 8750 1950 0    50   ~ 0
GP20: SPI0 RX  / UART1 TX  / I2C0 SDA / PWM2 A
Text Notes 8750 2050 0    50   ~ 0
GP21: SPI0 CS  / UART1 RX  / I2C0 SCL / PWM2 B
Text Notes 8750 2150 0    50   ~ 0
GP22: SPI0 SCK / UART1 CTS / I2C1 SDA / PWM3 A
Text Notes 8750 2250 0    50   ~ 0
GP23: SPI0 SCK / UART1 RTS / I2C1 SCL / PWM3 B
Text Notes 8750 2350 0    50   ~ 0
GP24: SPI1 RX  / UART1 TX  / I2C0 SDA / PWM4 A
Text Notes 8750 2450 0    50   ~ 0
GP25: SPI1 CS  / UART1 RX  / I2C0 SCL / PWM4 B
Text Notes 8750 2550 0    50   ~ 0
GP26: SPI1 SCK / UART1 CTS / I2C1 SDA / PWM5 A / ADC0
Text Notes 8750 2650 0    50   ~ 0
GP27: SPI1 TX  / UART1 RTS / I2C1 SCL / PWM5 B / ADC1
Text Notes 8750 2750 0    50   ~ 0
GP28: SPI1 RX  / UART0 TX  / I2C0 SDA / PWM6 A / ADC2
Text Notes 8750 2850 0    50   ~ 0
GP29: SPI1 CS  / UART0 RX  / I2C0 SCL / PWM6 B / ADC3
Text Notes 8750 1150 0    50   ~ 0
Vcc2 from BCU (5V)
$EndSCHEMATC

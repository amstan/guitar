EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:SparkFun
LIBS:guitar-components
LIBS:fret-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Guitar Fret"
Date "Tue Dec  2 2014"
Rev ""
Comp "Hypertriangle"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L C C7
U 1 1 547EB337
P 5250 2350
F 0 "C7" H 5300 2450 40  0000 L CNN
F 1 "100nF" H 5300 2250 40  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 5288 2200 30  0001 C CNN
F 3 "" H 5250 2350 60  0000 C CNN
F 4 "100nF" H 5250 2350 60  0001 C CNN "Digi-Key Page"
	1    5250 2350
	1    0    0    -1  
$EndComp
Text Label 5300 1400 0    60   ~ 0
BOOT0
$Comp
L C C11
U 1 1 547EBB23
P 5100 3750
F 0 "C11" H 5150 3850 40  0000 L CNN
F 1 "100nF" H 5150 3650 40  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 5138 3600 30  0001 C CNN
F 3 "" H 5100 3750 60  0000 C CNN
F 4 "100nF" H 5100 3750 60  0001 C CNN "Digi-Key Page"
	1    5100 3750
	1    0    0    -1  
$EndComp
$Comp
L R R6
U 1 1 547EBBEE
P 5100 3250
F 0 "R6" V 5180 3250 40  0000 C CNN
F 1 "10k" V 5107 3251 40  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 5030 3250 30  0001 C CNN
F 3 "" H 5100 3250 30  0000 C CNN
F 4 "10k" H 5100 3250 60  0001 C CNN "Digi-Key Page"
	1    5100 3250
	-1   0    0    1   
$EndComp
Text Label 5300 1300 0    60   ~ 0
NRST
$Comp
L GND #PWR01
U 1 1 547EC061
P 4950 2600
F 0 "#PWR01" H 4950 2600 30  0001 C CNN
F 1 "GND" H 4950 2530 30  0001 C CNN
F 2 "" H 4950 2600 60  0000 C CNN
F 3 "" H 4950 2600 60  0000 C CNN
	1    4950 2600
	1    0    0    -1  
$EndComp
$Comp
L C C6
U 1 1 547EC067
P 4950 2350
F 0 "C6" H 5000 2450 40  0000 L CNN
F 1 "100nF" H 5000 2250 40  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 4988 2200 30  0001 C CNN
F 3 "" H 4950 2350 60  0000 C CNN
F 4 "100nF" H 4950 2350 60  0001 C CNN "Digi-Key Page"
	1    4950 2350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR02
U 1 1 547EC833
P 5100 5550
F 0 "#PWR02" H 5100 5550 30  0001 C CNN
F 1 "GND" H 5100 5480 30  0001 C CNN
F 2 "" H 5100 5550 60  0000 C CNN
F 3 "" H 5100 5550 60  0000 C CNN
	1    5100 5550
	1    0    0    -1  
$EndComp
Text Label 5400 5300 2    60   ~ 0
SDA
Text Label 5400 5400 2    60   ~ 0
SCL
Text Label 7600 3750 2    60   ~ 0
SDA
$Comp
L R R7
U 1 1 547ECE9A
P 6350 5050
F 0 "R7" V 6430 5050 40  0000 C CNN
F 1 "4.7k" V 6357 5051 40  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 6280 5050 30  0001 C CNN
F 3 "" H 6350 5050 30  0000 C CNN
F 4 "4.7k" H 6350 5050 60  0001 C CNN "Digi-Key Page"
	1    6350 5050
	1    0    0    -1  
$EndComp
$Comp
L R R8
U 1 1 547ED325
P 6600 5050
F 0 "R8" V 6680 5050 40  0000 C CNN
F 1 "4.7k" V 6607 5051 40  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 6530 5050 30  0001 C CNN
F 3 "" H 6600 5050 30  0000 C CNN
F 4 "4.7k" H 6600 5050 60  0001 C CNN "Digi-Key Page"
	1    6600 5050
	1    0    0    -1  
$EndComp
Text Label 6350 5450 1    60   ~ 0
SDA
Text Label 6600 5450 1    60   ~ 0
SCL
$Comp
L WS2812 LED0
U 1 1 547EE0A6
P 2100 1350
F 0 "LED0" H 2100 1100 60  0000 C CNN
F 1 "WS2812" H 2100 1600 60  0000 C CNN
F 2 "guitar-components:5050-6pin" H 2100 1350 60  0001 C CNN
F 3 "" H 2100 1350 60  0000 C CNN
	1    2100 1350
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR03
U 1 1 547EE340
P 2750 1150
F 0 "#PWR03" H 2750 1240 20  0001 C CNN
F 1 "+5V" H 2750 1300 30  0000 C CNN
F 2 "" H 2750 1150 60  0000 C CNN
F 3 "" H 2750 1150 60  0000 C CNN
	1    2750 1150
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR04
U 1 1 547EE435
P 1200 1150
F 0 "#PWR04" H 1200 1110 30  0001 C CNN
F 1 "+3.3V" H 1200 1300 30  0000 C CNN
F 2 "" H 1200 1150 60  0000 C CNN
F 3 "" H 1200 1150 60  0000 C CNN
	1    1200 1150
	1    0    0    -1  
$EndComp
$Comp
L C C0
U 1 1 547EE5D6
P 1200 1600
F 0 "C0" H 1250 1700 40  0000 L CNN
F 1 "100nF" H 1250 1500 40  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 1238 1450 30  0001 C CNN
F 3 "" H 1200 1600 60  0000 C CNN
F 4 "100nF" H 1200 1600 60  0001 C CNN "Digi-Key Page"
	1    1200 1600
	1    0    0    -1  
$EndComp
Text Label 1300 1350 0    60   ~ 0
LED0
Text Label 7750 2000 2    60   ~ 0
LED0
Text Label 2950 1350 2    60   ~ 0
LED1
$Comp
L CONN_01X01 P0
U 1 1 547F2AD1
P 8200 1300
F 0 "P0" H 8350 1300 50  0000 C CNN
F 1 "TOUCH" V 8300 1300 50  0001 C CNN
F 2 "guitar-components:TP-8x15" H 8200 1300 60  0001 C CNN
F 3 "" H 8200 1300 60  0000 C CNN
F 4 "n/a" H 8200 1300 60  0001 C CNN "Digi-Key Page"
	1    8200 1300
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X01 P1
U 1 1 547F2EBD
P 8200 1400
F 0 "P1" H 8350 1400 50  0000 C CNN
F 1 "TOUCH" V 8300 1400 50  0001 C CNN
F 2 "guitar-components:TP-8x15" H 8200 1400 60  0001 C CNN
F 3 "" H 8200 1400 60  0000 C CNN
F 4 "n/a" H 8200 1400 60  0001 C CNN "Digi-Key Page"
	1    8200 1400
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X01 P2
U 1 1 547F2EDC
P 8200 1500
F 0 "P2" H 8350 1500 50  0000 C CNN
F 1 "TOUCH" V 8300 1500 50  0001 C CNN
F 2 "guitar-components:TP-8x15" H 8200 1500 60  0001 C CNN
F 3 "" H 8200 1500 60  0000 C CNN
F 4 "n/a" H 8200 1500 60  0001 C CNN "Digi-Key Page"
	1    8200 1500
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X01 P3
U 1 1 547F2EFB
P 8200 2250
F 0 "P3" H 8350 2250 50  0000 C CNN
F 1 "TOUCH" V 8300 2250 50  0001 C CNN
F 2 "guitar-components:TP-8x15" H 8200 2250 60  0001 C CNN
F 3 "" H 8200 2250 60  0000 C CNN
F 4 "n/a" H 8200 2250 60  0001 C CNN "Digi-Key Page"
	1    8200 2250
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X01 P4
U 1 1 547F2F1A
P 8200 2350
F 0 "P4" H 8350 2350 50  0000 C CNN
F 1 "TOUCH" V 8300 2350 50  0001 C CNN
F 2 "guitar-components:TP-8x15" H 8200 2350 60  0001 C CNN
F 3 "" H 8200 2350 60  0000 C CNN
F 4 "n/a" H 8200 2350 60  0001 C CNN "Digi-Key Page"
	1    8200 2350
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X01 P5
U 1 1 547F2F39
P 8200 2450
F 0 "P5" H 8350 2450 50  0000 C CNN
F 1 "TOUCH" V 8300 2450 50  0001 C CNN
F 2 "guitar-components:TP-8x15" H 8200 2450 60  0001 C CNN
F 3 "" H 8200 2450 60  0000 C CNN
F 4 "n/a" H 8200 2450 60  0001 C CNN "Digi-Key Page"
	1    8200 2450
	1    0    0    -1  
$EndComp
$Comp
L R R0
U 1 1 547F4407
P 7750 1300
F 0 "R0" V 7800 1450 40  0000 C CNN
F 1 "10k" V 7757 1301 40  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 7680 1300 30  0001 C CNN
F 3 "" H 7750 1300 30  0000 C CNN
F 4 "10k" H 7750 1300 60  0001 C CNN "Digi-Key Page"
	1    7750 1300
	0    -1   -1   0   
$EndComp
$Comp
L CONN_01X06 J10
U 1 1 547F6717
P 9800 5250
F 0 "J10" H 9800 5600 50  0000 C CNN
F 1 "CONN_01X06" V 9900 5250 50  0001 C CNN
F 2 "Connectors_Molex:Connector_Molex_PicoBlade_53261-0471" H 9800 5250 60  0001 C CNN
F 3 "" H 9800 5250 60  0000 C CNN
F 4 "todo" H 9800 5250 60  0001 C CNN "Digi-Key Page"
F 5 "Link TP Connector" H 9650 4700 60  0000 C CNN "Comment"
	1    9800 5250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR05
U 1 1 547F7BC1
P 9500 5600
F 0 "#PWR05" H 9500 5600 30  0001 C CNN
F 1 "GND" H 9500 5530 30  0001 C CNN
F 2 "" H 9500 5600 60  0000 C CNN
F 3 "" H 9500 5600 60  0000 C CNN
	1    9500 5600
	1    0    0    -1  
$EndComp
Text Label 9150 5100 0    60   ~ 0
SDA
Text Label 9150 5200 0    60   ~ 0
SCL
Text Label 9150 5400 0    60   ~ 0
TP_INT
Text Label 9150 5500 0    60   ~ 0
BUTTON
$Comp
L GNDPWR #PWR06
U 1 1 556A8D01
P 1500 1850
F 0 "#PWR06" H 1500 1900 40  0001 C CNN
F 1 "GNDPWR" H 1500 1770 40  0001 C CNN
F 2 "" H 1500 1850 60  0000 C CNN
F 3 "" H 1500 1850 60  0000 C CNN
	1    1500 1850
	1    0    0    -1  
$EndComp
$Comp
L GNDPWR #PWR07
U 1 1 556A8F28
P 4850 5550
F 0 "#PWR07" H 4850 5600 40  0001 C CNN
F 1 "GNDPWR" H 4850 5470 40  0001 C CNN
F 2 "" H 4850 5550 60  0000 C CNN
F 3 "" H 4850 5550 60  0000 C CNN
	1    4850 5550
	1    0    0    -1  
$EndComp
Text Label 8800 5500 0    60   ~ 0
LED5
Text Label 8800 5400 0    60   ~ 0
LED6
Text Label 7750 1900 2    60   ~ 0
BUTTON
Text Label 7750 1800 2    60   ~ 0
TP_INT
$Comp
L WS2812 LED1
U 1 1 556B2B80
P 2100 2400
F 0 "LED1" H 2100 2150 60  0000 C CNN
F 1 "WS2812" H 2100 2650 60  0000 C CNN
F 2 "guitar-components:5050-6pin" H 2100 2400 60  0001 C CNN
F 3 "" H 2100 2400 60  0000 C CNN
	1    2100 2400
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR08
U 1 1 556B2B86
P 2750 2200
F 0 "#PWR08" H 2750 2290 20  0001 C CNN
F 1 "+5V" H 2750 2350 30  0000 C CNN
F 2 "" H 2750 2200 60  0000 C CNN
F 3 "" H 2750 2200 60  0000 C CNN
	1    2750 2200
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR09
U 1 1 556B2B8C
P 1200 2200
F 0 "#PWR09" H 1200 2160 30  0001 C CNN
F 1 "+3.3V" H 1200 2350 30  0000 C CNN
F 2 "" H 1200 2200 60  0000 C CNN
F 3 "" H 1200 2200 60  0000 C CNN
	1    1200 2200
	1    0    0    -1  
$EndComp
$Comp
L C C1
U 1 1 556B2B92
P 1200 2650
F 0 "C1" H 1250 2750 40  0000 L CNN
F 1 "100nF" H 1250 2550 40  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 1238 2500 30  0001 C CNN
F 3 "" H 1200 2650 60  0000 C CNN
F 4 "100nF" H 1200 2650 60  0001 C CNN "Digi-Key Page"
	1    1200 2650
	1    0    0    -1  
$EndComp
Text Label 1300 2400 0    60   ~ 0
LED1
Text Label 2950 2400 2    60   ~ 0
LED2
$Comp
L GNDPWR #PWR010
U 1 1 556B2BAA
P 1500 2900
F 0 "#PWR010" H 1500 2950 40  0001 C CNN
F 1 "GNDPWR" H 1500 2820 40  0001 C CNN
F 2 "" H 1500 2900 60  0000 C CNN
F 3 "" H 1500 2900 60  0000 C CNN
	1    1500 2900
	1    0    0    -1  
$EndComp
$Comp
L WS2812 LED2
U 1 1 556B31BF
P 2100 3450
F 0 "LED2" H 2100 3200 60  0000 C CNN
F 1 "WS2812" H 2100 3700 60  0000 C CNN
F 2 "guitar-components:5050-6pin" H 2100 3450 60  0001 C CNN
F 3 "" H 2100 3450 60  0000 C CNN
	1    2100 3450
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR011
U 1 1 556B31C5
P 2750 3250
F 0 "#PWR011" H 2750 3340 20  0001 C CNN
F 1 "+5V" H 2750 3400 30  0000 C CNN
F 2 "" H 2750 3250 60  0000 C CNN
F 3 "" H 2750 3250 60  0000 C CNN
	1    2750 3250
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR012
U 1 1 556B31CB
P 1200 3250
F 0 "#PWR012" H 1200 3210 30  0001 C CNN
F 1 "+3.3V" H 1200 3400 30  0000 C CNN
F 2 "" H 1200 3250 60  0000 C CNN
F 3 "" H 1200 3250 60  0000 C CNN
	1    1200 3250
	1    0    0    -1  
$EndComp
$Comp
L C C2
U 1 1 556B31D1
P 1200 3700
F 0 "C2" H 1250 3800 40  0000 L CNN
F 1 "100nF" H 1250 3600 40  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 1238 3550 30  0001 C CNN
F 3 "" H 1200 3700 60  0000 C CNN
F 4 "100nF" H 1200 3700 60  0001 C CNN "Digi-Key Page"
	1    1200 3700
	1    0    0    -1  
$EndComp
Text Label 1300 3450 0    60   ~ 0
LED2
Text Label 2950 3450 2    60   ~ 0
LED3
$Comp
L GNDPWR #PWR013
U 1 1 556B31E9
P 1500 3950
F 0 "#PWR013" H 1500 4000 40  0001 C CNN
F 1 "GNDPWR" H 1500 3870 40  0001 C CNN
F 2 "" H 1500 3950 60  0000 C CNN
F 3 "" H 1500 3950 60  0000 C CNN
	1    1500 3950
	1    0    0    -1  
$EndComp
$Comp
L WS2812 LED3
U 1 1 556B31EF
P 2100 4500
F 0 "LED3" H 2100 4250 60  0000 C CNN
F 1 "WS2812" H 2100 4750 60  0000 C CNN
F 2 "guitar-components:5050-6pin" H 2100 4500 60  0001 C CNN
F 3 "" H 2100 4500 60  0000 C CNN
	1    2100 4500
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR014
U 1 1 556B31F5
P 2750 4300
F 0 "#PWR014" H 2750 4390 20  0001 C CNN
F 1 "+5V" H 2750 4450 30  0000 C CNN
F 2 "" H 2750 4300 60  0000 C CNN
F 3 "" H 2750 4300 60  0000 C CNN
	1    2750 4300
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR015
U 1 1 556B31FB
P 1200 4300
F 0 "#PWR015" H 1200 4260 30  0001 C CNN
F 1 "+3.3V" H 1200 4450 30  0000 C CNN
F 2 "" H 1200 4300 60  0000 C CNN
F 3 "" H 1200 4300 60  0000 C CNN
	1    1200 4300
	1    0    0    -1  
$EndComp
$Comp
L C C3
U 1 1 556B3201
P 1200 4750
F 0 "C3" H 1250 4850 40  0000 L CNN
F 1 "100nF" H 1250 4650 40  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 1238 4600 30  0001 C CNN
F 3 "" H 1200 4750 60  0000 C CNN
F 4 "100nF" H 1200 4750 60  0001 C CNN "Digi-Key Page"
	1    1200 4750
	1    0    0    -1  
$EndComp
Text Label 1300 4500 0    60   ~ 0
LED3
Text Label 2950 4500 2    60   ~ 0
LED4
$Comp
L GNDPWR #PWR016
U 1 1 556B3219
P 1500 5000
F 0 "#PWR016" H 1500 5050 40  0001 C CNN
F 1 "GNDPWR" H 1500 4920 40  0001 C CNN
F 2 "" H 1500 5000 60  0000 C CNN
F 3 "" H 1500 5000 60  0000 C CNN
	1    1500 5000
	1    0    0    -1  
$EndComp
$Comp
L WS2812 LED4
U 1 1 556B3740
P 2100 5550
F 0 "LED4" H 2100 5300 60  0000 C CNN
F 1 "WS2812" H 2100 5800 60  0000 C CNN
F 2 "guitar-components:5050-6pin" H 2100 5550 60  0001 C CNN
F 3 "" H 2100 5550 60  0000 C CNN
	1    2100 5550
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR017
U 1 1 556B3746
P 2750 5350
F 0 "#PWR017" H 2750 5440 20  0001 C CNN
F 1 "+5V" H 2750 5500 30  0000 C CNN
F 2 "" H 2750 5350 60  0000 C CNN
F 3 "" H 2750 5350 60  0000 C CNN
	1    2750 5350
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR018
U 1 1 556B374C
P 1200 5350
F 0 "#PWR018" H 1200 5310 30  0001 C CNN
F 1 "+3.3V" H 1200 5500 30  0000 C CNN
F 2 "" H 1200 5350 60  0000 C CNN
F 3 "" H 1200 5350 60  0000 C CNN
	1    1200 5350
	1    0    0    -1  
$EndComp
$Comp
L C C4
U 1 1 556B3752
P 1200 5800
F 0 "C4" H 1250 5900 40  0000 L CNN
F 1 "100nF" H 1250 5700 40  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 1238 5650 30  0001 C CNN
F 3 "" H 1200 5800 60  0000 C CNN
F 4 "100nF" H 1200 5800 60  0001 C CNN "Digi-Key Page"
	1    1200 5800
	1    0    0    -1  
$EndComp
Text Label 1300 5550 0    60   ~ 0
LED4
Text Label 2950 5550 2    60   ~ 0
LED5
$Comp
L GNDPWR #PWR019
U 1 1 556B376A
P 1500 6050
F 0 "#PWR019" H 1500 6100 40  0001 C CNN
F 1 "GNDPWR" H 1500 5970 40  0001 C CNN
F 2 "" H 1500 6050 60  0000 C CNN
F 3 "" H 1500 6050 60  0000 C CNN
	1    1500 6050
	1    0    0    -1  
$EndComp
$Comp
L WS2812 LED5
U 1 1 556B3770
P 2100 6600
F 0 "LED5" H 2100 6350 60  0000 C CNN
F 1 "WS2812" H 2100 6850 60  0000 C CNN
F 2 "guitar-components:5050-6pin" H 2100 6600 60  0001 C CNN
F 3 "" H 2100 6600 60  0000 C CNN
	1    2100 6600
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR020
U 1 1 556B3776
P 2750 6400
F 0 "#PWR020" H 2750 6490 20  0001 C CNN
F 1 "+5V" H 2750 6550 30  0000 C CNN
F 2 "" H 2750 6400 60  0000 C CNN
F 3 "" H 2750 6400 60  0000 C CNN
	1    2750 6400
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR021
U 1 1 556B377C
P 1200 6400
F 0 "#PWR021" H 1200 6360 30  0001 C CNN
F 1 "+3.3V" H 1200 6550 30  0000 C CNN
F 2 "" H 1200 6400 60  0000 C CNN
F 3 "" H 1200 6400 60  0000 C CNN
	1    1200 6400
	1    0    0    -1  
$EndComp
$Comp
L C C5
U 1 1 556B3782
P 1200 6850
F 0 "C5" H 1250 6950 40  0000 L CNN
F 1 "100nF" H 1250 6750 40  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 1238 6700 30  0001 C CNN
F 3 "" H 1200 6850 60  0000 C CNN
F 4 "100nF" H 1200 6850 60  0001 C CNN "Digi-Key Page"
	1    1200 6850
	1    0    0    -1  
$EndComp
Text Label 1300 6600 0    60   ~ 0
LED5
Text Label 2950 6600 2    60   ~ 0
LED6
$Comp
L GNDPWR #PWR022
U 1 1 556B379A
P 1500 7100
F 0 "#PWR022" H 1500 7150 40  0001 C CNN
F 1 "GNDPWR" H 1500 7020 40  0001 C CNN
F 2 "" H 1500 7100 60  0000 C CNN
F 3 "" H 1500 7100 60  0000 C CNN
	1    1500 7100
	1    0    0    -1  
$EndComp
$Comp
L R R1
U 1 1 556A92B2
P 7750 1400
F 0 "R1" V 7800 1550 40  0000 C CNN
F 1 "10k" V 7757 1401 40  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 7680 1400 30  0001 C CNN
F 3 "" H 7750 1400 30  0000 C CNN
F 4 "10k" H 7750 1400 60  0001 C CNN "Digi-Key Page"
	1    7750 1400
	0    -1   -1   0   
$EndComp
$Comp
L R R2
U 1 1 556A9304
P 7750 1500
F 0 "R2" V 7800 1650 40  0000 C CNN
F 1 "10k" V 7757 1501 40  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 7680 1500 30  0001 C CNN
F 3 "" H 7750 1500 30  0000 C CNN
F 4 "10k" H 7750 1500 60  0001 C CNN "Digi-Key Page"
	1    7750 1500
	0    -1   -1   0   
$EndComp
$Comp
L R R3
U 1 1 556A9380
P 7750 2250
F 0 "R3" V 7800 2400 40  0000 C CNN
F 1 "10k" V 7757 2251 40  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 7680 2250 30  0001 C CNN
F 3 "" H 7750 2250 30  0000 C CNN
F 4 "10k" H 7750 2250 60  0001 C CNN "Digi-Key Page"
	1    7750 2250
	0    -1   -1   0   
$EndComp
$Comp
L R R4
U 1 1 556A93CE
P 7750 2350
F 0 "R4" V 7800 2500 40  0000 C CNN
F 1 "10k" V 7757 2351 40  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 7680 2350 30  0001 C CNN
F 3 "" H 7750 2350 30  0000 C CNN
F 4 "10k" H 7750 2350 60  0001 C CNN "Digi-Key Page"
	1    7750 2350
	0    -1   -1   0   
$EndComp
$Comp
L R R5
U 1 1 556A9418
P 7750 2450
F 0 "R5" V 7800 2600 40  0000 C CNN
F 1 "10k" V 7757 2451 40  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 7680 2450 30  0001 C CNN
F 3 "" H 7750 2450 30  0000 C CNN
F 4 "10k" H 7750 2450 60  0001 C CNN "Digi-Key Page"
	1    7750 2450
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR023
U 1 1 556AADDA
P 5250 2600
F 0 "#PWR023" H 5250 2600 30  0001 C CNN
F 1 "GND" H 5250 2530 30  0001 C CNN
F 2 "" H 5250 2600 60  0000 C CNN
F 3 "" H 5250 2600 60  0000 C CNN
	1    5250 2600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR024
U 1 1 556AAE1B
P 5550 2600
F 0 "#PWR024" H 5550 2600 30  0001 C CNN
F 1 "GND" H 5550 2530 30  0001 C CNN
F 2 "" H 5550 2600 60  0000 C CNN
F 3 "" H 5550 2600 60  0000 C CNN
	1    5550 2600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR025
U 1 1 556AAE5C
P 5100 4000
F 0 "#PWR025" H 5100 4000 30  0001 C CNN
F 1 "GND" H 5100 3930 30  0001 C CNN
F 2 "" H 5100 4000 60  0000 C CNN
F 3 "" H 5100 4000 60  0000 C CNN
	1    5100 4000
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR026
U 1 1 556A838A
P 4850 4800
F 0 "#PWR026" H 4850 4890 20  0001 C CNN
F 1 "+5V" H 4850 4950 30  0000 C CNN
F 2 "" H 4850 4800 60  0000 C CNN
F 3 "" H 4850 4800 60  0000 C CNN
	1    4850 4800
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR027
U 1 1 556A8AE7
P 5100 4800
F 0 "#PWR027" H 5100 4760 30  0001 C CNN
F 1 "+3.3V" H 5100 4950 30  0000 C CNN
F 2 "" H 5100 4800 60  0000 C CNN
F 3 "" H 5100 4800 60  0000 C CNN
	1    5100 4800
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR028
U 1 1 556A8B5E
P 6350 4800
F 0 "#PWR028" H 6350 4760 30  0001 C CNN
F 1 "+3.3V" H 6350 4950 30  0000 C CNN
F 2 "" H 6350 4800 60  0000 C CNN
F 3 "" H 6350 4800 60  0000 C CNN
	1    6350 4800
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR029
U 1 1 556A8C92
P 6600 4800
F 0 "#PWR029" H 6600 4760 30  0001 C CNN
F 1 "+3.3V" H 6600 4950 30  0000 C CNN
F 2 "" H 6600 4800 60  0000 C CNN
F 3 "" H 6600 4800 60  0000 C CNN
	1    6600 4800
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR030
U 1 1 556A8CD3
P 9500 4900
F 0 "#PWR030" H 9500 4860 30  0001 C CNN
F 1 "+3.3V" H 9500 5050 30  0000 C CNN
F 2 "" H 9500 4900 60  0000 C CNN
F 3 "" H 9500 4900 60  0000 C CNN
	1    9500 4900
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X01 J1
U 1 1 556ACC73
P 4550 4900
F 0 "J1" H 4550 5000 50  0001 C CNN
F 1 "+5V" H 4650 4900 50  0000 L CNN
F 2 "guitar-components:IDC-1x1" H 4550 4900 60  0001 C CNN
F 3 "" H 4550 4900 60  0000 C CNN
F 4 "http://www.digikey.com/product-search/en?vendor=0&keywords=478-6141-1-ND" H 4550 4900 60  0001 C CNN "Digi-Key Page"
	1    4550 4900
	-1   0    0    1   
$EndComp
$Comp
L CONN_01X01 J2
U 1 1 556ADE23
P 4550 5000
F 0 "J2" H 4550 5100 50  0001 C CNN
F 1 "GNDPWR" H 4650 5000 50  0000 L CNN
F 2 "guitar-components:IDC-1x1" H 4550 5000 60  0001 C CNN
F 3 "" H 4550 5000 60  0000 C CNN
F 4 "http://www.digikey.com/product-search/en?vendor=0&keywords=478-6141-1-ND" H 4550 5000 60  0001 C CNN "Digi-Key Page"
	1    4550 5000
	-1   0    0    1   
$EndComp
$Comp
L CONN_01X01 J3
U 1 1 556ADE62
P 4550 5100
F 0 "J3" H 4550 5200 50  0001 C CNN
F 1 "+3.3V" H 4650 5100 50  0000 L CNN
F 2 "guitar-components:IDC-1x1" H 4550 5100 60  0001 C CNN
F 3 "" H 4550 5100 60  0000 C CNN
F 4 "http://www.digikey.com/product-search/en?vendor=0&keywords=478-6141-1-ND" H 4550 5100 60  0001 C CNN "Digi-Key Page"
	1    4550 5100
	-1   0    0    1   
$EndComp
$Comp
L CONN_01X01 J4
U 1 1 556ADEA1
P 4550 5200
F 0 "J4" H 4550 5300 50  0001 C CNN
F 1 "GND" H 4650 5200 50  0000 L CNN
F 2 "guitar-components:IDC-1x1" H 4550 5200 60  0001 C CNN
F 3 "" H 4550 5200 60  0000 C CNN
F 4 "http://www.digikey.com/product-search/en?vendor=0&keywords=478-6141-1-ND" H 4550 5200 60  0001 C CNN "Digi-Key Page"
	1    4550 5200
	-1   0    0    1   
$EndComp
$Comp
L CONN_01X01 J5
U 1 1 556ADEE0
P 4550 5300
F 0 "J5" H 4550 5400 50  0001 C CNN
F 1 "SDA" H 4650 5300 50  0000 L CNN
F 2 "guitar-components:IDC-1x1" H 4550 5300 60  0001 C CNN
F 3 "" H 4550 5300 60  0000 C CNN
F 4 "http://www.digikey.com/product-search/en?vendor=0&keywords=478-6141-1-ND" H 4550 5300 60  0001 C CNN "Digi-Key Page"
	1    4550 5300
	-1   0    0    1   
$EndComp
$Comp
L CONN_01X01 J6
U 1 1 556ADF1F
P 4550 5400
F 0 "J6" H 4550 5500 50  0001 C CNN
F 1 "SCL" H 4650 5400 50  0000 L CNN
F 2 "guitar-components:IDC-1x1" H 4550 5400 60  0001 C CNN
F 3 "" H 4550 5400 60  0000 C CNN
F 4 "http://www.digikey.com/product-search/en?vendor=0&keywords=478-6141-1-ND" H 4550 5400 60  0001 C CNN "Digi-Key Page"
	1    4550 5400
	-1   0    0    1   
$EndComp
$Comp
L CONN_01X01 J7
U 1 1 556B323A
P 4100 3150
F 0 "J7" H 4100 3250 50  0001 C CNN
F 1 "BOOT0" H 4200 3150 50  0000 L CNN
F 2 "guitar-components:pad-2mm-round" H 4100 3150 60  0001 C CNN
F 3 "" H 4100 3150 60  0000 C CNN
F 4 "n/a" H 4100 3150 60  0001 C CNN "Digi-Key Page"
	1    4100 3150
	-1   0    0    1   
$EndComp
$Comp
L CONN_01X01 J8
U 1 1 556B3279
P 4100 3250
F 0 "J8" H 4100 3350 50  0001 C CNN
F 1 "NRST" H 4200 3250 50  0000 L CNN
F 2 "guitar-components:pad-2mm-round" H 4100 3250 60  0001 C CNN
F 3 "" H 4100 3250 60  0000 C CNN
F 4 "n/a" H 4100 3250 60  0001 C CNN "Digi-Key Page"
	1    4100 3250
	-1   0    0    1   
$EndComp
Text Label 4650 3150 2    60   ~ 0
BOOT0
Text Label 4650 3250 2    60   ~ 0
NRST
$Comp
L +3.3V #PWR031
U 1 1 556B416D
P 5100 3000
F 0 "#PWR031" H 5100 2960 30  0001 C CNN
F 1 "+3.3V" H 5100 3150 30  0000 C CNN
F 2 "" H 5100 3000 60  0000 C CNN
F 3 "" H 5100 3000 60  0000 C CNN
	1    5100 3000
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR032
U 1 1 556AA489
P 1200 3950
F 0 "#PWR032" H 1200 3950 30  0001 C CNN
F 1 "GND" H 1200 3880 30  0001 C CNN
F 2 "" H 1200 3950 60  0000 C CNN
F 3 "" H 1200 3950 60  0000 C CNN
	1    1200 3950
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR033
U 1 1 556AA9AF
P 1200 1850
F 0 "#PWR033" H 1200 1850 30  0001 C CNN
F 1 "GND" H 1200 1780 30  0001 C CNN
F 2 "" H 1200 1850 60  0000 C CNN
F 3 "" H 1200 1850 60  0000 C CNN
	1    1200 1850
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR034
U 1 1 556AA9EE
P 1200 2900
F 0 "#PWR034" H 1200 2900 30  0001 C CNN
F 1 "GND" H 1200 2830 30  0001 C CNN
F 2 "" H 1200 2900 60  0000 C CNN
F 3 "" H 1200 2900 60  0000 C CNN
	1    1200 2900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR035
U 1 1 556AACA1
P 1200 5000
F 0 "#PWR035" H 1200 5000 30  0001 C CNN
F 1 "GND" H 1200 4930 30  0001 C CNN
F 2 "" H 1200 5000 60  0000 C CNN
F 3 "" H 1200 5000 60  0000 C CNN
	1    1200 5000
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR036
U 1 1 556AADFE
P 1200 6050
F 0 "#PWR036" H 1200 6050 30  0001 C CNN
F 1 "GND" H 1200 5980 30  0001 C CNN
F 2 "" H 1200 6050 60  0000 C CNN
F 3 "" H 1200 6050 60  0000 C CNN
	1    1200 6050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR037
U 1 1 556AAF41
P 1200 7100
F 0 "#PWR037" H 1200 7100 30  0001 C CNN
F 1 "GND" H 1200 7030 30  0001 C CNN
F 2 "" H 1200 7100 60  0000 C CNN
F 3 "" H 1200 7100 60  0000 C CNN
	1    1200 7100
	1    0    0    -1  
$EndComp
$Comp
L APE8865N-33-HF-3 U2
U 1 1 556BAE57
P 7800 5100
F 0 "U2" H 7800 5400 40  0000 C CNN
F 1 "AP2210N-3.3TRG1" H 7800 5300 40  0000 C CNN
F 2 "Housings_SOT-23_SOT-143_TSOT-6:SOT-23" H 7800 5200 35  0001 C CIN
F 3 "" H 7800 5100 60  0000 C CNN
F 4 "http://www.digikey.com/product-detail/en/AP2210N-3.3TRG1/AP2210N-3.3TRG1DICT-ND/4505294" H 7800 5100 60  0001 C CNN "Digi-Key Page"
	1    7800 5100
	1    0    0    -1  
$EndComp
$Comp
L C C9
U 1 1 556BB350
P 7250 5300
F 0 "C9" H 7300 5400 40  0000 L CNN
F 1 "22uF" H 7300 5200 40  0000 L CNN
F 2 "Capacitors_SMD:C_0805" H 7288 5150 30  0001 C CNN
F 3 "" H 7250 5300 60  0000 C CNN
F 4 "22nF" H 7250 5300 60  0001 C CNN "Digi-Key Page"
	1    7250 5300
	1    0    0    -1  
$EndComp
$Comp
L C C10
U 1 1 556BB592
P 8350 5300
F 0 "C10" H 8400 5400 40  0000 L CNN
F 1 "22uF" H 8400 5200 40  0000 L CNN
F 2 "Capacitors_SMD:C_0805" H 8388 5150 30  0001 C CNN
F 3 "" H 8350 5300 60  0000 C CNN
F 4 "22nF" H 8350 5300 60  0001 C CNN "Digi-Key Page"
	1    8350 5300
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR038
U 1 1 556BB724
P 8350 5550
F 0 "#PWR038" H 8350 5550 30  0001 C CNN
F 1 "GND" H 8350 5480 30  0001 C CNN
F 2 "" H 8350 5550 60  0000 C CNN
F 3 "" H 8350 5550 60  0000 C CNN
	1    8350 5550
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR039
U 1 1 556BB897
P 7250 4950
F 0 "#PWR039" H 7250 5040 20  0001 C CNN
F 1 "+5V" H 7250 5100 30  0000 C CNN
F 2 "" H 7250 4950 60  0000 C CNN
F 3 "" H 7250 4950 60  0000 C CNN
	1    7250 4950
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR040
U 1 1 556BBA78
P 8350 4950
F 0 "#PWR040" H 8350 4910 30  0001 C CNN
F 1 "+3.3V" H 8350 5100 30  0000 C CNN
F 2 "" H 8350 4950 60  0000 C CNN
F 3 "" H 8350 4950 60  0000 C CNN
	1    8350 4950
	1    0    0    -1  
$EndComp
$Comp
L GNDPWR #PWR041
U 1 1 556BC043
P 7250 5550
F 0 "#PWR041" H 7250 5600 40  0001 C CNN
F 1 "GNDPWR" H 7250 5470 40  0001 C CNN
F 2 "" H 7250 5550 60  0000 C CNN
F 3 "" H 7250 5550 60  0000 C CNN
	1    7250 5550
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR042
U 1 1 556BC414
P 7800 5500
F 0 "#PWR042" H 7800 5500 30  0001 C CNN
F 1 "GND" H 7800 5430 30  0001 C CNN
F 2 "" H 7800 5500 60  0000 C CNN
F 3 "" H 7800 5500 60  0000 C CNN
	1    7800 5500
	1    0    0    -1  
$EndComp
$Comp
L JUMPER JP1
U 1 1 556BC964
P 7800 5900
F 0 "JP1" H 7800 6050 50  0000 C CNN
F 1 "GNDPWR-GND" H 7800 5820 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" H 7800 5900 60  0001 C CNN
F 3 "" H 7800 5900 60  0000 C CNN
F 4 "0ohm" H 7800 5900 60  0001 C CNN "Digi-Key Page"
	1    7800 5900
	1    0    0    -1  
$EndComp
$Comp
L GNDPWR #PWR043
U 1 1 556BCD95
P 7400 6000
F 0 "#PWR043" H 7400 6050 40  0001 C CNN
F 1 "GNDPWR" H 7400 5920 40  0001 C CNN
F 2 "" H 7400 6000 60  0000 C CNN
F 3 "" H 7400 6000 60  0000 C CNN
	1    7400 6000
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR044
U 1 1 556BCE54
P 8200 6000
F 0 "#PWR044" H 8200 6000 30  0001 C CNN
F 1 "GND" H 8200 5930 30  0001 C CNN
F 2 "" H 8200 6000 60  0000 C CNN
F 3 "" H 8200 6000 60  0000 C CNN
	1    8200 6000
	1    0    0    -1  
$EndComp
Text Notes 8400 6350 0    60   ~ 0
Optional
$Comp
L STM32F051K U1
U 1 1 5587512D
P 6500 2500
F 0 "U1" H 6500 2250 60  0000 C CNN
F 1 "STM32F051K*T" H 6500 2150 60  0000 C CNN
F 2 "Housings_QFP:LQFP-32_7x7mm_Pitch0.8mm" H 6500 2500 60  0001 C CNN
F 3 "" H 6500 2500 60  0000 C CNN
F 4 "http://www.digikey.com/product-detail/en/STM32F051K8T6/497-13626-ND/3947378" H 6500 2500 60  0001 C CNN "Digi-Key Page"
	1    6500 2500
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR045
U 1 1 55875AE1
P 4650 2600
F 0 "#PWR045" H 4650 2600 30  0001 C CNN
F 1 "GND" H 4650 2530 30  0001 C CNN
F 2 "" H 4650 2600 60  0000 C CNN
F 3 "" H 4650 2600 60  0000 C CNN
	1    4650 2600
	1    0    0    -1  
$EndComp
$Comp
L C C8
U 1 1 55875AE7
P 4650 2350
F 0 "C8" H 4700 2450 40  0000 L CNN
F 1 "100nF" H 4700 2250 40  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 4688 2200 30  0001 C CNN
F 3 "" H 4650 2350 60  0000 C CNN
F 4 "100nF" H 4650 2350 60  0001 C CNN "Digi-Key Page"
	1    4650 2350
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR046
U 1 1 55875AED
P 4650 1800
F 0 "#PWR046" H 4650 1760 30  0001 C CNN
F 1 "+3.3V" H 4650 1950 30  0000 C CNN
F 2 "" H 4650 1800 60  0000 C CNN
F 3 "" H 4650 1800 60  0000 C CNN
	1    4650 1800
	1    0    0    -1  
$EndComp
NoConn ~ 5650 1600
NoConn ~ 5650 1700
Text Label 7700 2750 2    60   ~ 0
SWCLK
Text Label 7700 2850 2    60   ~ 0
SWDIO
Text Label 4650 3050 2    60   ~ 0
SWCLK
Text Label 4650 2950 2    60   ~ 0
SWDIO
Text Label 4800 3500 0    60   ~ 0
NRST
$Comp
L R R12
U 1 1 558794E0
P 5400 3750
F 0 "R12" V 5480 3750 40  0000 C CNN
F 1 "10k" V 5407 3751 40  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 5330 3750 30  0001 C CNN
F 3 "" H 5400 3750 30  0000 C CNN
F 4 "10k" H 5400 3750 60  0001 C CNN "Digi-Key Page"
	1    5400 3750
	-1   0    0    1   
$EndComp
$Comp
L GND #PWR047
U 1 1 558798A8
P 5400 4000
F 0 "#PWR047" H 5400 4000 30  0001 C CNN
F 1 "GND" H 5400 3930 30  0001 C CNN
F 2 "" H 5400 4000 60  0000 C CNN
F 3 "" H 5400 4000 60  0000 C CNN
	1    5400 4000
	1    0    0    -1  
$EndComp
Text Label 5400 3250 3    60   ~ 0
BOOT0
$Comp
L CONN_01X01 J11
U 1 1 5587A2CE
P 4100 3050
F 0 "J11" H 4100 3150 50  0001 C CNN
F 1 "SWCLK" H 4200 3050 50  0000 L CNN
F 2 "guitar-components:pad-2mm-round" H 4100 3050 60  0001 C CNN
F 3 "" H 4100 3050 60  0000 C CNN
F 4 "n/a" H 4100 3050 60  0001 C CNN "Digi-Key Page"
	1    4100 3050
	-1   0    0    1   
$EndComp
$Comp
L CONN_01X01 J9
U 1 1 5587A325
P 4100 2950
F 0 "J9" H 4100 3050 50  0001 C CNN
F 1 "SWDIO" H 4200 2950 50  0000 L CNN
F 2 "guitar-components:pad-2mm-round" H 4100 2950 60  0001 C CNN
F 3 "" H 4100 2950 60  0000 C CNN
F 4 "n/a" H 4100 2950 60  0001 C CNN "Digi-Key Page"
	1    4100 2950
	-1   0    0    1   
$EndComp
Text Label 7950 3350 2    60   ~ 0
SCL
Text Label 8550 3550 2    60   ~ 0
SDA
Text Label 7600 3650 2    60   ~ 0
SCL
Text Label 7600 3550 2    60   ~ 0
MOSI
Text Label 7600 3450 2    60   ~ 0
MISO
Text Label 7600 3350 2    60   ~ 0
SCK
Text Notes 6600 3500 0    60   ~ 0
SPI1
Text Notes 6200 3750 0    60   ~ 0
I2C1/USART1
Text Notes 7600 3750 0    60   ~ 0
/RX
Text Notes 7600 3650 0    60   ~ 0
/TX
Text Notes 6450 2050 0    60   ~ 0
SPI1_MOSI
Wire Wire Line
	5650 2400 5550 2400
Wire Wire Line
	5550 2300 5550 2600
Wire Wire Line
	5650 2300 5550 2300
Connection ~ 5550 2400
Wire Wire Line
	5250 2500 5250 2600
Wire Wire Line
	5300 1400 5650 1400
Wire Wire Line
	4650 2000 5650 2000
Wire Wire Line
	5300 1300 5650 1300
Wire Wire Line
	5100 3900 5100 4000
Wire Wire Line
	4950 2500 4950 2600
Connection ~ 4950 2000
Wire Wire Line
	4650 2100 5650 2100
Connection ~ 5250 2100
Wire Wire Line
	4850 4800 4850 4900
Wire Wire Line
	4850 4900 4750 4900
Wire Wire Line
	4750 5100 5100 5100
Wire Wire Line
	5100 5100 5100 4800
Wire Wire Line
	4750 5000 4850 5000
Wire Wire Line
	4850 5000 4850 5550
Wire Wire Line
	4750 5200 5100 5200
Wire Wire Line
	5100 5200 5100 5550
Wire Wire Line
	5400 5300 4750 5300
Wire Wire Line
	5400 5400 4750 5400
Wire Wire Line
	7600 3650 7350 3650
Wire Wire Line
	7600 3750 7350 3750
Wire Wire Line
	6350 5450 6350 5200
Wire Wire Line
	6600 5450 6600 5200
Wire Wire Line
	6600 4800 6600 4900
Wire Wire Line
	6350 4800 6350 4900
Wire Wire Line
	1200 1250 1600 1250
Wire Wire Line
	1200 1150 1200 1450
Wire Wire Line
	1500 1450 1500 1850
Wire Wire Line
	1500 1450 1600 1450
Wire Wire Line
	1200 1850 1200 1750
Connection ~ 1200 1250
Wire Wire Line
	2650 1250 2750 1250
Wire Wire Line
	2750 1250 2750 1150
Wire Wire Line
	1300 1350 1600 1350
Wire Wire Line
	7750 2000 7350 2000
Wire Wire Line
	2950 1350 2650 1350
Wire Wire Line
	9150 5100 9600 5100
Wire Wire Line
	9150 5200 9600 5200
Wire Wire Line
	8800 5400 9600 5400
Wire Wire Line
	8800 5500 9600 5500
Wire Wire Line
	9500 4900 9500 5000
Wire Wire Line
	9500 5000 9600 5000
Wire Wire Line
	9500 5600 9500 5300
Wire Wire Line
	9500 5300 9600 5300
Connection ~ 9100 5400
Connection ~ 9100 5500
Wire Wire Line
	7350 1800 7750 1800
Wire Wire Line
	7350 1900 7750 1900
Wire Wire Line
	1200 2300 1600 2300
Wire Wire Line
	1200 2200 1200 2500
Wire Wire Line
	1500 2500 1500 2900
Wire Wire Line
	1500 2500 1600 2500
Wire Wire Line
	1200 2900 1200 2800
Connection ~ 1200 2300
Wire Wire Line
	2650 2300 2750 2300
Wire Wire Line
	2750 2300 2750 2200
Wire Wire Line
	1300 2400 1600 2400
Wire Wire Line
	2950 2400 2650 2400
Wire Wire Line
	1200 3350 1600 3350
Wire Wire Line
	1200 3250 1200 3550
Wire Wire Line
	1500 3550 1500 3950
Wire Wire Line
	1500 3550 1600 3550
Wire Wire Line
	1200 3950 1200 3850
Connection ~ 1200 3350
Wire Wire Line
	2650 3350 2750 3350
Wire Wire Line
	2750 3350 2750 3250
Wire Wire Line
	1300 3450 1600 3450
Wire Wire Line
	2950 3450 2650 3450
Wire Wire Line
	1200 4400 1600 4400
Wire Wire Line
	1200 4300 1200 4600
Wire Wire Line
	1500 4600 1500 5000
Wire Wire Line
	1500 4600 1600 4600
Wire Wire Line
	1200 5000 1200 4900
Connection ~ 1200 4400
Wire Wire Line
	2650 4400 2750 4400
Wire Wire Line
	2750 4400 2750 4300
Wire Wire Line
	1300 4500 1600 4500
Wire Wire Line
	2950 4500 2650 4500
Wire Wire Line
	1200 5450 1600 5450
Wire Wire Line
	1200 5350 1200 5650
Wire Wire Line
	1500 5650 1500 6050
Wire Wire Line
	1500 5650 1600 5650
Wire Wire Line
	1200 6050 1200 5950
Connection ~ 1200 5450
Wire Wire Line
	2650 5450 2750 5450
Wire Wire Line
	2750 5450 2750 5350
Wire Wire Line
	1300 5550 1600 5550
Wire Wire Line
	2950 5550 2650 5550
Wire Wire Line
	1200 6500 1600 6500
Wire Wire Line
	1200 6400 1200 6700
Wire Wire Line
	1500 6700 1500 7100
Wire Wire Line
	1500 6700 1600 6700
Wire Wire Line
	1200 7100 1200 7000
Connection ~ 1200 6500
Wire Wire Line
	2650 6500 2750 6500
Wire Wire Line
	2750 6500 2750 6400
Wire Wire Line
	1300 6600 1600 6600
Wire Wire Line
	2950 6600 2650 6600
Wire Wire Line
	4300 3150 4650 3150
Wire Wire Line
	5100 3400 5100 3600
Wire Wire Line
	5100 3000 5100 3100
Connection ~ 5100 3500
Wire Wire Line
	7250 4950 7250 5150
Wire Wire Line
	7350 5050 7250 5050
Connection ~ 7250 5050
Wire Wire Line
	8350 4950 8350 5150
Wire Wire Line
	8250 5050 8350 5050
Connection ~ 8350 5050
Wire Wire Line
	7250 5450 7250 5550
Wire Wire Line
	8350 5450 8350 5550
Wire Wire Line
	7800 5400 7800 5500
Wire Wire Line
	8100 5900 8200 5900
Wire Wire Line
	8200 5900 8200 6000
Wire Wire Line
	7500 5900 7400 5900
Wire Wire Line
	7400 5900 7400 6000
Wire Notes Line
	6900 4650 6900 6200
Wire Notes Line
	6900 6200 10300 6200
Wire Notes Line
	10300 6200 10300 4650
Wire Notes Line
	10300 4650 6900 4650
Wire Notes Line
	8600 4650 8600 6200
Wire Wire Line
	4650 2500 4650 2600
Wire Wire Line
	4650 1800 4650 2200
Connection ~ 4650 2000
Connection ~ 4650 2100
Wire Wire Line
	4650 1900 5650 1900
Connection ~ 4650 1900
Wire Wire Line
	4950 2000 4950 2200
Wire Wire Line
	5250 2200 5250 2100
Wire Wire Line
	7350 2750 7700 2750
Wire Wire Line
	7700 2850 7350 2850
Wire Wire Line
	4300 3050 4650 3050
Wire Wire Line
	4650 2950 4300 2950
Wire Wire Line
	4300 3250 4650 3250
Wire Wire Line
	4800 3500 5100 3500
Wire Wire Line
	5400 3900 5400 4000
Wire Wire Line
	5400 3250 5400 3600
Wire Wire Line
	7350 3350 7950 3350
Wire Wire Line
	8550 3550 8350 3550
Wire Wire Line
	7350 3550 7750 3550
Wire Wire Line
	7350 3450 7650 3450
Wire Notes Line
	6900 3350 6850 3350
Wire Notes Line
	6850 3350 6850 3550
Wire Notes Line
	6850 3550 6900 3550
Wire Notes Line
	6900 3450 6850 3450
Wire Notes Line
	6900 3650 6850 3650
Wire Notes Line
	6850 3650 6850 3750
Wire Notes Line
	6850 3750 6900 3750
Wire Notes Line
	6900 1600 6850 1600
Wire Notes Line
	6850 1600 6850 1300
Wire Notes Line
	6850 1300 6900 1300
Wire Notes Line
	6900 1500 6850 1500
Wire Notes Line
	6850 1400 6900 1400
Text Notes 6500 1500 0    60   ~ 0
TSC_G1
Text Notes 6500 2450 0    60   ~ 0
TSC_G4
Wire Notes Line
	6900 2550 6850 2550
Wire Notes Line
	6850 2550 6850 2250
Wire Notes Line
	6850 2250 6900 2250
Wire Notes Line
	6900 2350 6850 2350
Wire Notes Line
	6900 2450 6850 2450
$Comp
L C C12
U 1 1 55888064
P 8000 1850
F 0 "C12" H 8050 1950 40  0000 L CNN
F 1 "47nF" H 8050 1750 40  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 8038 1700 30  0001 C CNN
F 3 "" H 8000 1850 60  0000 C CNN
F 4 "47nF" H 8000 1850 60  0001 C CNN "Digi-Key Page"
	1    8000 1850
	1    0    0    -1  
$EndComp
$Comp
L C C13
U 1 1 558883FF
P 8000 2800
F 0 "C13" H 8050 2900 40  0000 L CNN
F 1 "47nF" H 8050 2700 40  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 8038 2650 30  0001 C CNN
F 3 "" H 8000 2800 60  0000 C CNN
F 4 "47nF" H 8000 2800 60  0001 C CNN "Digi-Key Page"
	1    8000 2800
	1    0    0    -1  
$EndComp
Wire Wire Line
	7350 2550 8000 2550
Wire Wire Line
	8000 2550 8000 2650
Wire Wire Line
	7350 1600 8000 1600
Wire Wire Line
	8000 1600 8000 1700
$Comp
L GND #PWR048
U 1 1 558888C8
P 8000 3000
F 0 "#PWR048" H 8000 3000 30  0001 C CNN
F 1 "GND" H 8000 2930 30  0001 C CNN
F 2 "" H 8000 3000 60  0000 C CNN
F 3 "" H 8000 3000 60  0000 C CNN
	1    8000 3000
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR049
U 1 1 558889CF
P 8000 2050
F 0 "#PWR049" H 8000 2050 30  0001 C CNN
F 1 "GND" H 8000 1980 30  0001 C CNN
F 2 "" H 8000 2050 60  0000 C CNN
F 3 "" H 8000 2050 60  0000 C CNN
	1    8000 2050
	1    0    0    -1  
$EndComp
Wire Wire Line
	8000 2950 8000 3000
Wire Wire Line
	8000 2000 8000 2050
Wire Wire Line
	7350 1300 7600 1300
Wire Wire Line
	7350 1400 7600 1400
Wire Wire Line
	7350 1500 7600 1500
Wire Wire Line
	7900 1300 8000 1300
Wire Wire Line
	8000 1400 7900 1400
Wire Wire Line
	7900 1500 8000 1500
Wire Wire Line
	7900 2250 8000 2250
Wire Wire Line
	8000 2350 7900 2350
Wire Wire Line
	7900 2450 8000 2450
Wire Wire Line
	7350 2450 7600 2450
Wire Wire Line
	7350 2350 7600 2350
Wire Wire Line
	7350 2250 7600 2250
Wire Wire Line
	7650 3450 7650 3550
Connection ~ 7650 3550
$Comp
L JUMPER JP2
U 1 1 55878349
P 8050 3550
F 0 "JP2" H 8050 3700 50  0000 C CNN
F 1 "SPI-CONNECTION" H 8050 3470 50  0001 C CNN
F 2 "Resistors_SMD:R_0603" H 8050 3550 60  0001 C CNN
F 3 "" H 8050 3550 60  0000 C CNN
F 4 "0ohm" H 8050 3550 60  0001 C CNN "Digi-Key Page"
	1    8050 3550
	1    0    0    -1  
$EndComp
NoConn ~ 7350 1700
NoConn ~ 7350 2150
NoConn ~ 7350 2650
NoConn ~ 7350 3050
NoConn ~ 7350 3150
Text Label 7400 1300 0    60   ~ 0
P0
Text Label 7400 1400 0    60   ~ 0
P1
Text Label 7400 1500 0    60   ~ 0
P2
Text Label 7400 1600 0    60   ~ 0
CG1
Text Label 7400 2250 0    60   ~ 0
P3
Text Label 7400 2350 0    60   ~ 0
P4
Text Label 7400 2450 0    60   ~ 0
P5
Text Label 7400 2550 0    60   ~ 0
CG4
Connection ~ 7750 3350
$EndSCHEMATC

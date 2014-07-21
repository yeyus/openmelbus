EESchema Schematic File Version 2  date jue 23 ene 2014 23:17:18 PST
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
LIBS:special
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
EELAYER 43  0
EELAYER END
$Descr A4 11700 8267
encoding utf-8
Sheet 1 1
Title ""
Date "23 jan 2014"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L 4066 U?
U 4 1 52E0CC27
P 6850 6150
F 0 "U?" H 7050 6001 40  0000 C CNN
F 1 "4066" H 7050 6300 40  0000 C CNN
	4    6850 6150
	1    0    0    -1  
$EndComp
$Comp
L 4066 U?
U 2 1 52E0CC22
P 6850 5250
F 0 "U?" H 7050 5101 40  0000 C CNN
F 1 "4066" H 7050 5400 40  0000 C CNN
	2    6850 5250
	1    0    0    -1  
$EndComp
$Comp
L 4066 U?
U 3 1 52E0CC17
P 4950 6150
F 0 "U?" H 5150 6001 40  0000 C CNN
F 1 "4066" H 5150 6300 40  0000 C CNN
	3    4950 6150
	1    0    0    -1  
$EndComp
$Comp
L 4066 U?
U 1 1 52E0CC05
P 4950 5250
F 0 "U?" H 5150 5101 40  0000 C CNN
F 1 "4066" H 5150 5400 40  0000 C CNN
	1    4950 5250
	1    0    0    -1  
$EndComp
$Comp
L ATMEGA328-A IC1
U 1 1 52E0C9FD
P 5900 2950
F 0 "IC1" H 5200 4200 50  0000 L BNN
F 1 "ATMEGA328-A" H 6150 1550 50  0000 L BNN
F 2 "TQFP32" H 5350 1600 50  0001 C CNN
	1    5900 2950
	1    0    0    -1  
$EndComp
Wire Wire Line
	1250 2250 1650 2250
Wire Wire Line
	1250 1650 1650 1650
Wire Wire Line
	1650 1950 1250 1950
$Comp
L R R3
U 1 1 52E0C962
P 1900 2250
F 0 "R3" V 1980 2250 50  0000 C CNN
F 1 "100" V 1900 2250 50  0000 C CNN
	1    1900 2250
	0    1    1    0   
$EndComp
$Comp
L R R2
U 1 1 52E0C953
P 1900 1950
F 0 "R2" V 1980 1950 50  0000 C CNN
F 1 "100" V 1900 1950 50  0000 C CNN
	1    1900 1950
	0    1    1    0   
$EndComp
$Comp
L R R1
U 1 1 52E0C942
P 1900 1650
F 0 "R1" V 1980 1650 50  0000 C CNN
F 1 "100" V 1900 1650 50  0000 C CNN
	1    1900 1650
	0    1    1    0   
$EndComp
Text GLabel 1250 3600 0    60   Input ~ 12
VOICE
Text GLabel 1250 4100 0    60   Input ~ 12
GND
Text GLabel 1250 4350 0    60   Input ~ 12
RUN/VBAT
Text GLabel 1250 3350 0    60   Input ~ 12
RIGHT
Text GLabel 1250 3100 0    60   Input ~ 12
LEFT
Text GLabel 1250 2850 0    60   Input ~ 12
AGND
Text GLabel 1250 2250 0    60   Input ~ 12
MCLK
Text GLabel 1250 1950 0    60   BiDi ~ 12
MBUSY
Text GLabel 1250 1650 0    60   BiDi ~ 12
MDATA
$EndSCHEMATC

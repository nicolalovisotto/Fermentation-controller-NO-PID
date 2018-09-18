# buildings


Software for fermentation processes control in chiller systems.

Components list:
- Arduino Mega2560
- LCD display 20x4
- DS3231 real time clock (RTC)
- 3 normally opened buttons (NO)
- ADS1115 16 bit (ADC)
- Eliwell PTC temperature probe.

Suits for:
- cooling
- heating
- fermentation chamber ventilation
- CPU ventilation.

Working parameters can be activated/deactivated and modified from LCD display.

Next steps:
1) set up sketch for PT100/NTC temperature probe
2) code optimization switching global variables with local variables
3) interface development to display and modify real time data via Bluetooth and/or web
4) PCB circuit.

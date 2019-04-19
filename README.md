# Fermentation controller NO PID


Software designed for cooling and heating fermentation control in refrigerators and freezers.

Main components list:
- NodeMCU ESP-12E
- LCD display 20x4
- DS3231 real time clock (RTC)
- ADS1115 16 bit (ADC)
- PT100 class A temperature probe
- TMP36 temperature sensor.

Suits for:
- cooling
- heating
- fermentation chamber ventilation
- CPU ventilation.

Working parameters can be enabled, disabled or modified from LCD display or Blynk.

Next steps:
1) add a function to secure setpoints gap
2) add offset to temperature
3) board always on with other devices shutdown/enable
4) board deep sleep function
5) interface development to display and modify real time data and/or web
6) wiring diagram.

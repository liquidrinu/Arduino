
### Required:
- NodeMcu lolin devkit V3
- custom DrinuTech PCB
- 3x 5mm leds (blue, red, green)
- 3x 220 ohm resistor
- 1x 10k resistor
- 1x hygrometer
- 1x DHT 11
- 20x4 i2c LCD


*custom PCB needs VIN to vcc on LCD I2C module (5v) and SDA -> SCL on Pcb and SCL -> SDA*

Download LCD library from here : https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library

#### Main code file is MAIN.ino

lcd currently unstable and messes up some readings from DHT11, solved with software hack, but keep this in mind that reading are not absolute

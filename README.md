
### Required:
- NodeMcu lolin devkit V3
- custom DrinuTech PCB
- 3x 5mm leds (blue, red, green)
- 3x 220 ohm resistor
- 1x 10k resistor
- 1x hygrometer
- 1x DHT 11
- 20x4 i2c LCD


### Quick Start

1.) custom PCB needs VIN to VCC on LCD I2C module (5v).

2.) SDA -> SCL on Pcb and SCL -> SDA (SDA and SCL wires basically need to be swapped)

3.) Enter your own SSID and PASSWORD in the "MAIN.ino" file

4.) upload the code
* hold flash on the NodeMcu if upload gives error *

5.) Readings can be found on http://192.168.1.x/plant (where 'x' is the ip assigned by your network)

Download LCD library from here : https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library

#### Main code file is MAIN.ino

lcd currently unstable and messes up readings here and there from DHT11, solved with software hack, but keep this in mind that reading are not absolute, but doable.

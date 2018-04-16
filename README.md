
### Required:
- NodeMcu lolin devkit V3
- custom DrinuTech PCB
- 3x 5mm leds (blue, red, green)
- 3x 220 ohm resistor
- 1x 10k resistor
- 1x hygrometer
- 1x DHT 11
- 1x LCD (20x4 I2C)


### Quick Start

1.) custom PCB VIN -> LCD VCC on I2C module (5v). 3.3v inadequate

2.) SDA -> SCL on Pcb and SCL -> SDA (SDA and SCL wires basically need to be swapped)

3.) Enter your own SSID and PASSWORD in the "MAIN.ino" file

4.) upload the code
* hold flash on the NodeMcu if upload gives error *

5.) Readings can be found on http://192.168.1.x/plant (where 'x' is the ip assigned by your network)

Download LCD library from here : https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library

#### Main code file is MAIN.ino

lcd currently unstable and readings from the DHT11 can be 1 minute off, solved with software hack to keep throughput (memoization), but do keep this in mind.

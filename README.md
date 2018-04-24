# Plant-o-meter V1

<img src="https://github.com/liquidrinu/Arduino/blob/master/plantometer_pcb.png"  width="400">

### Required:
- NodeMcu lolin devkit V3
- custom DrinuTech PCB
- 3x 5mm leds (blue, red, green)
- 3x 220 ohm resistor
- 1x 10k resistor
- 1x hygrometer
- 1x DHT 11
- 1x LCD (20x4 I2C)

Download LCD library from here : https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library

# (highly recommended) D8_hack.ino

Current setup corrodes the soil meter real fast.

The way to fix this is to wire pin **D8** to the **Vcc** of the soilmeter module, then upload **D8_hack.ino** on to the nodemcu.

** cut one end of a female dupont cable, then solder one end to the pin on the board, and plug the other onto the header**

This code is also an upgrade to the "MAIN.ino" file


### Quick Start

###### Main code file is MAIN.ino

1.) Solder/connect *VIN* ('Power +' pin on pcb) to LCD *VCC* on the I2C module. 
###### The pcb states 3.3V, but it's 5V from the USB

2.) SDA and SCL wires need to be swapped around

3.) Enter your own SSID and PASSWORD in the "MAIN.ino" or "D8_hack.ino" file

4.) upload the code
```
- Board NodeMCU 1.0 (ESP-12E module)
- uploadspeed 115200 baud
```
* hold flash on the NodeMcu if upload gives error
* different boards might require ESP-12 library (0.9) and/or different baudrate (9600)

5.) Readings can be found on http://192.168.1.x/plant (where 'x' is the ip assigned by your network)

*Readings from the DHT11 can be 1 minute off, but solved with software hack to keep continuous output (memoization), but do keep this in mind.*

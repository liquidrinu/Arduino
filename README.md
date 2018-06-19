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

## (highly recommended) D8_PORTAL.ino / D8_hack.ino 

**D8_PORTAL is same source as D8_hack.ino but with a webportal for credentials**

Current setup corrodes the soil meter real fast.

The way to fix this is to wire pin **D8** to the **Vcc** of the soilmeter module, then upload **D8_hack.ino/D8_PORTAL.ino** on to the nodemcu.

** cut one end of a female dupont cable, then solder the sliced end to the pin on the board, and plug the female connecter on the **Vcc** header of the **plant sensor module** **

Improvements:

- async (except during actual read phase of the soil, which gets full priority);
- more efficient http request
- more responsive capacitive touch switch (stdby)
- more power efficient
- decreased corrosion
- code clean-up

# Get Started

###### Main code file is MAIN.ino (deprecated use D8_hack.ino/D8_PORTAL.ino)
###### The pcb states 3.3V, but it's 5V from the USB

1.) Solder/connect *VIN* ('Power +' pin on pcb) to LCD *VCC* on the I2C module. 

2.) SDA and SCL wires need to be swapped around (SDA -> SCL and vice versa)

3.) Enter your own SSID and PASSWORD in the "MAIN.ino" or "D8_hack.ino" file.

4.) upload the code
```
- Board NodeMCU 1.0 (ESP-12E module)
- uploadspeed 115200 baud
```
* hold flash on the NodeMcu if upload gives error
* different boards might require ESP-12 library (0.9) and/or different baudrate (9600)

5.) Readings can be found on http://192.168.1.x/plant (where 'x' is the ip assigned by your network)

*Readings from the DHT11 can be 1 minute off, but solved with software hack to keep continuous output (memoization), but do keep this in mind.*

## D8_PORTAL (important)

1.) Upload both 'D8_PORTAL.ino' and 'index.h' ( both in same folder) and re-plug the board.

2.) Connect through wifi on your phone to the esp8266 and use passwd 12345678.

3.) it should automatically ask for redirection to a captive webportal page.

4.) Click "Configure new AP" and choose your local network.

5.) You can now select from http://192.168.1.x when connected to the local network.

6.) Change AP settings at http://192.168.1.x/_ac


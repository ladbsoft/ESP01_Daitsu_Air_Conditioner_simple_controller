# ESP01 MQTT Daitsu Air Conditioner simple controller
### A small sketch that lets you control a Daitsu air conditioner (in my case, model DSG-07HRN2) over MQTT using a small ESP-01 board

![Logo](https://raw.githubusercontent.com/ladbsoft/ESP01_Daitsu_Air_Conditioner_simple_controller/master/Image.png "Logo")

## About this
In the long journey of modifying my old appliances to be a bit more smart (because if they work, why would I replace them? 😉), this time I designed this small sketch that lets me use my air conditioner from any MQTT application, in my case, OpenHAB. It lets you change the ON/OFF state, the temperature (17 to 30 degrees or AUTO), the mode (AUTO, COOL, FAN, or HEAT) and speed (AUTO, LOW, MED or HIGH).

The sketch uses [WiFiManager](https://github.com/tzapu/WiFiManager) to allow the WiFi configuration in case it changes without having to reprogram the board.

## Installation
You'll need:

### Hardware
- ESP-01 or similar microcontroller from the ESP8266 family
- A way of assembling the microcontroller to an infrared LED and a small power supply. In my case, I used [this schematic](https://github.com/ladbsoft/ESP01_Daitsu_Air_Conditioner_simple_controller/blob/master/ESP01_Air_Conditioner_simple_controller_Schematic.zip)


### Software
- [Arduino IDE](https://www.arduino.cc/en/Main/Software)
- The [ESP8266WiFi](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi) library
- The [PubSubClient](https://github.com/knolleary/pubsubclient) library
- The [ESP8266 WiFiManager](https://github.com/tzapu/WiFiManager) library
- An MQTT server to communicate to (in my case I used the embedded MQTT server from [OpenHAB](https://www.openhab.org/)

### Configuration of the Arduino Sketch
You'll need to configure in Configuration.h the IP, user, and password of your MQTT server, the client ID, and the eight MQTT topics to use and an SSID and password to create a new WiFi connection with a captive portal, that lets you connect to your WiFi router. This is not your actual WiFi router SSID and password, but a new one. Once you power on the microcontroller for the first time, connect to the SSID you configured here with your password and navigate to the captive portal to then configure your real WiFi connection.

More info (only in Spanish for now) in https://elprogramadoriberico.es/2019/05/14/haciendo-inteligente-mi-aire-acondicionado-parte-1/

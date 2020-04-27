# ESP01 MQTT Daitsu Air Conditioner simple controller
## A small sketch that lets you control a Daitsu air conditioner (in my case, model DSG-07HRN2) over MQTT using a small ESP-01 board

In the long journey of modifying my old appliances to be a bit more smart (because if they work, why would I replace them? 😉), this time I designed this small sketch that lets me use my air conditioner from any MQTT application, in my case, OpenHAB. It lets you change the ON/OFF state, the temperature (17 to 30 degrees or AUTO), the mode (AUTO, COOL, FAN, or HEAT) and speed (AUTO, LOW, MED or HIGH).

The sketch uses [WiFiManager](https://github.com/tzapu/WiFiManager) to allow the WiFi configuration in case it changes without having to reprogram the board.

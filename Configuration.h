// +------------------------------------------------------------------+
// |                       C O N S T A N T S                          |
// +------------------------------------------------------------------+

//MQTT
const char* mqttServer            = "<YourMQTTServerIP>";
const int   mqttPort              = 1883;
const char* mqttUser              = "<YourMQTTUser>";
const char* mqttPassword          = "<YourMQTTPassword>";
const char* mqttClientId          = "<ANewMQTTClientID>";
const char* mqttPowerStateTopic   = "<ANewMQTTTopic>";
const char* mqttPowerCommandTopic = "<ANewMQTTTopic2>";
const char* mqttModeStateTopic    = "<ANewMQTTTopic3>";
const char* mqttModeCommandTopic  = "<ANewMQTTTopic4>";
const char* mqttTempStateTopic    = "<ANewMQTTTopic5>";
const char* mqttTempCommandTopic  = "<ANewMQTTTopic6>";
const char* mqttSpeedStateTopic   = "<ANewMQTTTopic7>";
const char* mqttSpeedCommandTopic = "<ANewMQTTTopic8>";

//GPIO
const byte  IRReceiverPin = 1; //TX
const byte  IRSenderPin   = 3; //RX

// WI-FI CONFIGURATION NETWORK. IF THE ESP01 CANNOT CONNECT TO THE
// LAST WI-FI SPOT USED, A NEW CONNECTION WITH THIS SSID AND
// PASSWORD IS CREATED ON THE ESP01, SO YOU CAN CONNECT TO IT AND
// CONFIGURE THE NEW CONNECTION
const char* wifiSsid     = "<NewSSID>";
const char* wifiPassword = "<NewPassword>";

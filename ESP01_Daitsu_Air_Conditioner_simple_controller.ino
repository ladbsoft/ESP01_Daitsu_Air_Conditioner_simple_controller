// #==================================================================#
// ‖ Author: Luis Alejandro Domínguez Bueno (LADBSoft)                ‖
// ‖ Date: 2019-03-20                                   Version: 0.1a ‖
// #==================================================================#
// ‖ Name: ESP8266 MQTT daitsu air conditioner simple controller      ‖
// ‖ Description: A sketch for the ESP8266 (ESP-01 to be exact) for   ‖
// ‖ adding IoT functionality to a daitsu classic air conditioner.    ‖
// ‖ The desired functions are ON, OFF, set mode, set speed and set   ‖
// ‖ temperature.                                                     ‖
// ‖ The microcontroller will be connected to an MQTT server, for     ‖
// ‖ sending status and receiving orders.                             ‖
// ‖ The original idea is to receive signals from the air conditioner ‖
// ‖ IR sensor and intercept them, change status according to them,   ‖
// ‖ and resend them directly to the original controller board, but   ‖
// ‖ a version not-so intrusive is also possible, blocking the        ‖
// ‖ original IR sensor with tape, putting a new one for the          ‖
// ‖ microcontroller, and hooking an IR LED just in front of the      ‖
// ‖ original sensor (beneath the tape).                              ‖
// #==================================================================#
// ‖ Version history:                                                 ‖
// #==================================================================#
// ‖ 0.1a: Start of development. Connection to the MQTT server.       ‖
// ‖ Original remote command replication.                             ‖
// #==================================================================#

// +------------------------------------------------------------------+
// |                       C O N S T A N T S                          |
// +------------------------------------------------------------------+
const char* mqttClientId   = "FF_MasterBedroomAir";
const char* mqttPowerStateTopic = "Home/FF_MasterBedroom/AirConditioner/PowerS";
const char* mqttPowerCommandTopic = "Home/FF_MasterBedroom/AirConditioner/PowerC";
const char* mqttModeStateTopic  = "Home/FF_MasterBedroom/AirConditioner/ModeS";
const char* mqttModeCommandTopic  = "Home/FF_MasterBedroom/AirConditioner/ModeC";
const char* mqttTempStateTopic  = "Home/FF_MasterBedroom/AirConditioner/TemperatureS";
const char* mqttTempCommandTopic  = "Home/FF_MasterBedroom/AirConditioner/TemperatureC";
const char* mqttSpeedStateTopic = "Home/FF_MasterBedroom/AirConditioner/SpeedS";
const char* mqttSpeedCommandTopic = "Home/FF_MasterBedroom/AirConditioner/SpeedC";
const byte  IRReceiverPin = 1; //TX
const byte  IRSenderPin = 3; //RX

// +------------------------------------------------------------------+
// |                        I N C L U D E S                           |
// +------------------------------------------------------------------+
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include "Connection.h"
#include "Commands.h"

// +------------------------------------------------------------------+
// |                         G L O B A L S                            |
// +------------------------------------------------------------------+

WiFiClient espClient;
PubSubClient client(espClient);
long lastCheck = 0;

// +------------------------------------------------------------------+
// |                           S E T U P                              |
// +------------------------------------------------------------------+

void setup() {
  //Disable Serial pins in order to use them as GPIO
  pinMode(1, FUNCTION_3); //TX
  pinMode(3, FUNCTION_3); //RX

  pinMode(IRReceiverPin, INPUT);   // For receiving remote commands
  pinMode(IRSenderPin, OUTPUT);    // For sending commands

  commandSetup();

  setup_wifi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

// +------------------------------------------------------------------+
// |                            L O O P                               |
// +------------------------------------------------------------------+

void loop() {
  String payload;
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastCheck > 5000) {
    lastCheck = now;

    if(powerState) {
      client.publish(mqttPowerStateTopic, "ON");
    } else {
      client.publish(mqttPowerStateTopic, "OFF");
    }
  }
}

// +------------------------------------------------------------------+
// |                     S U B R O U T I N E S                        |
// +------------------------------------------------------------------+

void setup_wifi() {
  delay(10);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String topicString = String(topic);
  byte* payloadZeroTerm = (byte*)malloc(length+1);
  String payloadString;
  byte payloadByte;

// Conversion of payload to String
  memcpy(payloadZeroTerm, payload, length);
  payloadZeroTerm[length] = '\0';
  payloadString = String((char*)payloadZeroTerm);

// Conversion of payload to Byte
  payloadByte = (byte)payloadString.toInt();

// Power Topic: Payload will be "ON" or "OFF"
  if(topicString.equals(String(mqttPowerCommandTopic))) {
    if (payloadString.equals("ON")) {
      sendPowerCommand(true);
    } else if (payloadString.equals("OFF")) {
      sendPowerCommand(false);
    }

// Speed Topic: Payload will be a number between 0 (AUTO) and 3 (HIGH)
  } else if(topicString.equals(String(mqttSpeedCommandTopic))) {
    if(payloadByte >= 0 && payloadByte <= 3) {
      sendSpeedCommand(payloadByte);
    }

// Mode Topic: Payload will be a number:
// 0 -> AUTO
// 1 -> COOL
// 2 -> FAN
// 3 -> HEAT
  } else if(topicString.equals(String(mqttModeCommandTopic))) {
    if(payloadByte >= 0 && payloadByte <= 3) {
      sendModeCommand(payloadByte);
    }

// Temperature Topic: Payload will be a number corresponding with the desired temperature, between 17 and 30
  } else if(topicString.equals(String(mqttTempCommandTopic))) {
    if(payloadByte >= 17 && payloadByte <= 30) {
      sendTemperatureCommand(payloadByte);
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    // Attempt to connect
    if (client.connect(mqttClientId, mqttUser, mqttPassword)) {
      // Once connected, resubscribe
      client.subscribe(mqttPowerCommandTopic);
      client.subscribe(mqttModeCommandTopic);
      client.subscribe(mqttTempCommandTopic);
      client.subscribe(mqttSpeedCommandTopic);
    } else {
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

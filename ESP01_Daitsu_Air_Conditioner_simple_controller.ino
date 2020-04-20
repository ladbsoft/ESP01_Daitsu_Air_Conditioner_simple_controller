// #==================================================================#
// ‖ Author: Luis Alejandro Domínguez Bueno (LADBSoft)                ‖
// ‖ Date: 2020-04-20                                   Version: 0.4a ‖
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
// ‖ 0.4a: Added WiFi Manager, to enable new WiFi configuration       ‖
// ‖ without reprogramming.                                           ‖
// ‖ 0.3a: Bug fixes. All commands seem to work fine.                 ‖
// ‖ 0.2a: Complete command support. Still a bit buggy though.        ‖
// ‖ 0.1a: Start of development. Connection to the MQTT server.       ‖
// ‖ Original remote command replication.                             ‖
// #==================================================================#

// +------------------------------------------------------------------+
// |                        I N C L U D E S                           |
// +------------------------------------------------------------------+
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include "Configuration.h"
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

  setup_wifi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  commandSetup();
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

    publishStates();
  }
}

// +------------------------------------------------------------------+
// |                     S U B R O U T I N E S                        |
// +------------------------------------------------------------------+

void setup_wifi() {
  WiFiManager wifiManager;
  wifiManager.setTimeout(180); //3 minutes

  if(!wifiManager.autoConnect(wifiSsid, wifiPassword)) {
    //Retry after 3 minutes with no WiFi connection
    ESP.reset();
    delay(5000);
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

// Speed Topic: Payload will be "AUTO", "LOW", "MED", or "HIGH"
  } else if(topicString.equals(String(mqttSpeedCommandTopic))) {
    if (payloadString.equals("AUTO")) {
      sendSpeedCommand(0);
    } else if (payloadString.equals("LOW")) {
      sendSpeedCommand(1);
    } else if (payloadString.equals("MED")) {
      sendSpeedCommand(2);
    } else if (payloadString.equals("HIGH")) {
      sendSpeedCommand(3);
    }

// Mode Topic: Payload will be "AUTO", "COOL", "FAN" or "HEAT"
  } else if(topicString.equals(String(mqttModeCommandTopic))) {
    if (payloadString.equals("AUTO")) {
      sendModeCommand(0);
    } else if (payloadString.equals("COOL")) {
      sendModeCommand(1);
    } else if (payloadString.equals("FAN")) {
      sendModeCommand(2);
    } else if (payloadString.equals("HEAT")) {
      sendModeCommand(3);
    }

// Temperature Topic: Payload will be "AUTO" or a number corresponding to the desired temperature, between 17 and 30
  } else if(topicString.equals(String(mqttTempCommandTopic))) {
    if (payloadString.equals("AUTO")) {
      sendTemperatureCommand(0);
    } else {
      if (payloadByte >= 17 && payloadByte <= 30) {
        sendTemperatureCommand(payloadByte);
      }
    }
  }

  publishStates();
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

void publishStates() {
//  Publish power state
  if (powerState) {
    client.publish(mqttPowerStateTopic, "ON");
  } else {
    client.publish(mqttPowerStateTopic, "OFF");
  }

//  Publish mode
  if (mode == 0) {
    client.publish(mqttModeStateTopic, "AUTO");
  } else if (mode == 1) {
    client.publish(mqttModeStateTopic, "COOL");
  } else if (mode == 2) {
    client.publish(mqttModeStateTopic, "FAN");
  } else if (mode == 3) {
    client.publish(mqttModeStateTopic, "HEAT");
  }

//  Publish temperature
  if (temperature == 0) {
    client.publish(mqttTempStateTopic, "AUTO");
  } else {
    client.publish(mqttTempStateTopic, String(temperature).c_str());
  }

//  Publish speed
  if (fanSpeed == 0) {
    client.publish(mqttSpeedStateTopic, "AUTO");
  } else if (fanSpeed == 1) {
    client.publish(mqttSpeedStateTopic, "LOW");
  } else if (fanSpeed == 2) {
    client.publish(mqttSpeedStateTopic, "MED");
  } else if (fanSpeed == 3) {
    client.publish(mqttSpeedStateTopic, "HIGH");
  } else if (fanSpeed == 4) {
    client.publish(mqttSpeedStateTopic, "SPECIAL");
  } else if (fanSpeed == 5) {
    client.publish(mqttSpeedStateTopic, "OFF");
  }
}

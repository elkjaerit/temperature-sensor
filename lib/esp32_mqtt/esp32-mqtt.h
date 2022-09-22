/******************************************************************************
   Copyright 2018 Google
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 *****************************************************************************/
// This file contains static methods for API requests using Wifi / MQTT
#ifndef __ESP32_MQTT_H__
#define __ESP32_MQTT_H__

#include <Client.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include <esp_int_wdt.h>
#include <esp_task_wdt.h>

#include <MQTT.h>

#include <CloudIoTCore.h>
#include <CloudIoTCoreMqtt.h>
#include "ciotc_config.h" // Update this file with your configuration

// !!REPLACEME!!
// The MQTT callback function for commands and configuration updates
// Place your message handler code here.
void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
}
///////////////////////////////

// Initialize WiFi and MQTT for this board
WiFiClientSecure *netClient;
CloudIoTCoreDevice *device;
CloudIoTCoreMqtt *mqtt;
MQTTClient *mqttClient;
unsigned long iat = 0;
String jwt;

String getJwt() {
  iat = time(nullptr);
  Serial.println("Refreshing JWT");
  jwt = device->createJWT(iat, jwt_exp_secs);
  return jwt;
}

void hard_restart() {
  esp_task_wdt_init(1, true);
  esp_task_wdt_add(NULL);
  while (true);
}

void connectWifi() {
  Serial.print("checking wifi...");

  // https://github.com/knolleary/pubsubclient/issues/624
  long wifidelay = 0;
  for (int k = 0; k < 25; k++){
    if ( WiFi.status() != WL_CONNECTED ){
      wifidelay = wifidelay + 250;
      WiFi.disconnect();
      delay(wifidelay);
      Serial.println("XXXXXXXXXXXXXXXX---------Connecting to WiFi...-------------XXXXXXXXXXXXX");
      WiFi.mode(WIFI_STA);
      delay(wifidelay);
      WiFi.begin(ssid, password);
      delay(wifidelay);
      delay(wifidelay);
    }
  }
  if ( WiFi.status() != WL_CONNECTED )
  {
    Serial.println("Connecting to WiFi fail.");
    delay (10000);
 //   hard_restart();
    // I think that is no longer necessary, but it does not hurt either.    
  }
  if ( WiFi.status() == WL_CONNECTED )
  {
    Serial.println("");
    Serial.println("WiFi Connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.printf("RSSi: %ld dBm\n", WiFi.RSSI());

    configTime(0, 0, ntp_primary, ntp_secondary);
    Serial.println("Waiting on time sync...");
    while (time(nullptr) < 1510644967) {
      delay(10);
    }
    Serial.println("Timesync done");
  }
}

///////////////////////////////
// Orchestrates various methods from preceeding code.
///////////////////////////////
bool publishTelemetry(String data) {
  return mqtt->publishTelemetry(data);
}

bool publishTelemetry(const char* data, int length) {
  return mqtt->publishTelemetry(data, length);
}

bool publishTelemetry(String subfolder, String data) {
  return mqtt->publishTelemetry(subfolder, data);
}

bool publishTelemetry(String subfolder, const char* data, int length) {
  return mqtt->publishTelemetry(subfolder, data, length);
}

void connect() {
  connectWifi();
  // Changed this to false (from nothing)
  mqtt->mqttConnect(); // maybe use false here ?
}

void setupCloudIoT() {
  device = new CloudIoTCoreDevice(
    project_id, location, registry_id, device_id,
    private_key_str);

  connectWifi();
  netClient = new WiFiClientSecure();
  netClient->setCACert(root_cert);
  mqttClient = new MQTTClient(512);
  mqttClient->setOptions(180, true, 1000); // keepAlive, cleanSession, timeout
  mqtt = new CloudIoTCoreMqtt(mqttClient, netClient, device);
  mqtt->setUseLts(true);
  mqtt->startMQTT();
}
#endif //__ESP32_MQTT_H__
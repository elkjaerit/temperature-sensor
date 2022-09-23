#include <Arduino.h>
#include <esp_task_wdt.h>
#include "ATC_MiThermometer.h"

#include "esp32-mqtt.h"

const int scanTime = 5; // BLE scan time in seconds

ATC_MiThermometer miThermometer;

void setup()
{
  Serial.begin(115200);
  esp_task_wdt_init(120, true);
  // Initialization
  setupCloudIoT();
  esp_task_wdt_reset();
  if (!mqttClient->connected())
  {
    connect();
  }
  miThermometer.begin();
}

String getDeviceId()
{
  String chipId = String((uint32_t)ESP.getEfuseMac(), HEX);
  chipId.toUpperCase();
  return chipId;
}

void sendBattery(char *macAddress, double batt, double rssi, long timestamp)
{

  char data[150] = {};
  sprintf(data, "{\"deviceId\": \"%s\", \"mac\": \"%s\", \"batt\": %.1f, \"rssi\": %.0f, \"timestamp\": %d}", getDeviceId().c_str(), macAddress, batt, rssi, timestamp);
  Serial.println(data);

  publishTelemetry(data);
}

void sendTempAndHumidity(char *macAddress, double temperature, double humidity, double rssi, long timestamp)
{
  char data[150] = {};
  sprintf(data, "{\"deviceId\": \"%s\", \"mac\": \"%s\", \"temp\": %.1f, \"humidity\": %.1f, \"rssi\": %.0f, \"timestamp\": %d}", getDeviceId().c_str(), macAddress, temperature, humidity, rssi, timestamp);
  Serial.println(data);

  publishTelemetry(data);
}

void loop()
{

  esp_task_wdt_add(NULL);
  mqtt->loop();
  delay(10);
  esp_task_wdt_reset();
  delay(100); // <- fixes some issues with WiFi stability

  if (!mqttClient->connected())
  {
    Serial.println("Mqtt client not connected - reconnect");
    connect();
  }

  esp_task_wdt_reset();

  // Set sensor data invalid
  miThermometer.resetData();

  // Get sensor data - run BLE scan for <scanTime>
  unsigned found = miThermometer.getData(scanTime);

  for (int i = 0; i < miThermometer.data.size(); i++)
  {
    if (miThermometer.data[i].valid)
    {
      unsigned long timestamp = time(nullptr);

      String name = miThermometer.data[i].name.c_str();
      name.toUpperCase();

      char *macAddress = const_cast<char *>(name.c_str());

      if (miThermometer.data[i].batt_level > 0)
      {
        sendBattery(macAddress, miThermometer.data[i].batt_level, miThermometer.data[i].rssi, timestamp);
      }

      if (miThermometer.data[i].temperature > 0)
      {
        sendTempAndHumidity(macAddress, miThermometer.data[i].temperature / 100.0, miThermometer.data[i].humidity / 100.0, miThermometer.data[i].rssi, timestamp);
      }

      Serial.println();
    }
  }
  Serial.print("Devices found: ");
  Serial.println(found);
  Serial.println();

  // Delete results fromBLEScan buffer to release memory
  miThermometer.clearScanResults();
  delay(5000);
}

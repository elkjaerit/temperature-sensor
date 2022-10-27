#include <Arduino.h>
#include <esp_task_wdt.h>
#include "ATC_MiThermometer.h"

#include "esp32-mqtt.h"

const int scanTime = 15; // BLE scan time in seconds

ATC_MiThermometer miThermometer;

void setup()
{
  Serial.begin(115200);

// Set watch dog timeout to 1+ second of scan time
  esp_task_wdt_init(3 + 1, true);

  setupCloudIoT();

  esp_task_wdt_reset();

  if (!mqttClient->connected())
  {
    connect();
  }
  miThermometer.begin();
}

void loop()
{
  Serial.println("*************************************");
  Serial.println("Start scanning");
  Serial.println("*************************************");
  esp_task_wdt_add(NULL);

  delay(10);
  
  delay(100); // <- fixes some issues with WiFi stability

  esp_task_wdt_init(3 + 1, true);
  esp_task_wdt_reset();

  if (!mqttClient->connected())
  {
    Serial.println("Mqtt client not connected - reconnect");
    connect();
  }

  mqtt->loop();

  esp_task_wdt_reset();

  // Set sensor data invalid
  miThermometer.resetData();

  esp_task_wdt_init(scanTime + 1, true);
  esp_task_wdt_reset();

  // Get sensor data - run BLE scan for <scanTime>
  miThermometer.getData(scanTime);

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
    }
  }

  // Delete results fromBLEScan buffer to release memory
  miThermometer.clearScanResults();

  Serial.println("*************************************");
  Serial.println("Scanning ended");
  Serial.println("*************************************");
  
  delay(100);
}

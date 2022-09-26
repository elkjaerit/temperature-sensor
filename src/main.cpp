#include <Arduino.h>
#include <esp_task_wdt.h>

#include "esp32-mqtt.h"

void setup()
{
  Serial.begin(115200);

  esp_task_wdt_init(120, true);

  setupCloudIoT();

  esp_task_wdt_reset();

  if (!mqttClient->connected())
  {
    connect();
  }
}

void loop()
{
  Serial.println("*************************************");
  Serial.println("Start scanning");
  Serial.println("*************************************");
  esp_task_wdt_add(NULL);

  delay(10);
  esp_task_wdt_reset();
  delay(100); // <- fixes some issues with WiFi stability

  if (!mqttClient->connected())
  {
    Serial.println("Mqtt client not connected - reconnect");
    connect();
  }

  mqtt->loop();

  esp_task_wdt_reset();

  // Set sensor data invalid

  unsigned long timestamp = time(nullptr);
  
  Serial.println(WiFi.macAddress());
  //sendTempAndHumidity(WiFi.macAddress(), 25.0, 70.0, 100, timestamp);

  delay(5000);
}

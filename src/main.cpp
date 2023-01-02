#include <Arduino.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <esp_task_wdt.h>
#include "ATC_MiThermometer.h"
#include <ArduinoJson.h>

#include <addons/TokenHelper.h>

#include <HTTPClient.h>

#include "time.h"

#include "utils.h"
#include "config.h"
#include "jsonutils.h"
#include "publisher.h"

FirebaseAuth auth;
FirebaseConfig config;

const int scanTime = 30; // BLE scan time in seconds
const int sleeptime = 60 * 3 - scanTime;
const int watchDogFeedInterval = scanTime + sleeptime + 10;

ATC_MiThermometer miThermometer;

uint32_t resetAfterMillis = 24 * 60 * 60 * 1000;
uint32_t lastResetWas;

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

void printLocalTime()
{
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void scanAndUploadData()
{
  // Get sensor data
  miThermometer.resetData();
  miThermometer.getData(scanTime);

  String sensorDataAsJson = buildJson(miThermometer.data);

  // Delete results fromBLEScan buffer to release memory
  miThermometer.clearScanResults();

  Serial.println("Scanning ended...");

  Serial.println(sensorDataAsJson);

  send(sensorDataAsJson, String(Firebase.getToken()));
}

void setup()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP

  // WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;
  digitalWrite(LED_BUILTIN, LOW); // turn the LED off

  // wm.resetSettings();

  wm.setConfigPortalTimeout(180);

  if (!wm.autoConnect("SmartHeatingAP"))
  {
    Serial.println("Failed to connect");
    resetESP();
  }
  else
  {
    // if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    printLocalTime();

    /* Assign the sevice account credentials and private key (required) */
    config.service_account.data.client_email = "esp-temp-sensor@smart-heating-1.iam.gserviceaccount.com";
    config.service_account.data.project_id = "smart-heating-1";
    config.service_account.data.private_key = "-----BEGIN PRIVATE KEY-----\nMIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQDBU44J69xWE3IU\nmrh0Qon7z4gSWPqrjkKJFIyfLuHaSOg/F/WhSWTJLph+QfmRK5jZJ9E2FVIDWYyZ\nzWIK7tyJ/MWclv50csbGJxe6zcL6Gve5qZG96hDN2+IM4h1wEbrBMsJbPspwJwQF\n/Y4j41NoWIxf2YWS0BFbEBRaQckmPi3W1XhtMz38K/1C7E0uyFJ70B+EK1/DUGJc\n0t1drRdDOT5Zqse94Y+oQiU6EmginAXYQuEgUKWrDMktT7W9HoCKNFB94p/2+0Wu\nJUVGjtPsgu0ZyHr5rGEIXq5EZZGb4x8Xj6Yy8BWgx5TWfs4EHc32zOvMi2pMILJR\nlWgKvQUdAgMBAAECggEAHXglILIJZj2UmDt1fpNeATqDXXiezWPkFI1DWkiFOQcA\nEA4Q7UMIz/oAWM6pwk1JAWEmbP4XAFe4LQG3vj7m80nKvbHum8WbMz1mSp1u6T0P\nIggNJUv2v8qSzK/zGNA7DZQ1OD98EFiCLn+WGTZf0noc+8dmqxh4bM8oqxF4HhH8\nJHvlpiuPXvO//QxccGxnfRNdZCTZFfqdBMPVy/JCwuMBJwUWjb40KCLmOquMnxO2\nof35jhzLZyZPqnoDc+9Urz3itJ0n6LlDvUHHdnUXTtlsJiDfgmUBHEp4GfRvuzDG\n/n/vNEyERmFtlUXd7dj71GxXlroejuSDiYw90a/EwQKBgQD4Hhv09+7KNon4L8fJ\naJKowBl9VlCrwD4ogNIXhHuc9UpENl88Z/lvweEuS1wFRnTXlk4aslnqCf6wGk0n\ngli9FnrgnAIQV7cO6CoFXE40cr3A8qGW1gvXRNeycT82Lbre9p3VDEc7qlUYwtgS\nAWApi+Rro7H2doEZ/EjiMX7zFQKBgQDHd9btpiFOPjLHzx+OdzW3bkA7LTuwtfId\n/qCCM83TwnwfPj9BQZeCSym01FLf2IFxC6pesdzdC964jEdWKXsTFPI5iw+lxAjn\nhotzxncBrLH6mr+hEe6oYPtdf7Lyh0794RztSC9fCghlQIKfa3mWv6xh8feBCAb1\nMuo9Ojhr6QKBgAyhO7sUg8s3S36esATIle6RGLsQkbqsZn2ZURhxXfl3Yvhl7CMf\nB8twiw3YOC7sjzYKKJ+jRIBtUdGVBShlsi6t3kNgrZo3XNIdb0YmlLSGwrH3p9IN\nwzyJ/JDQwu73FPQUiaQ2o8mdugcwo98GwuZagJ4aDw0Eqz2vYeZhrpyhAoGAMpNP\nOriZz0X0CDebL7tIMndQ7/A7J0Yq2rIaLolGEgBJCn70+O3Rpaa7L296h+lUgL5N\nd28vqhh9Y1umJtGjtCXmePKqiARZoaK6ryNOP21zPzCHkCaE1cqpUX60d8wZGkQ+\n2mcoI7fegxXFSPiuPqvg4IDCmcPYR8meHOIrNikCgYEAgvId6EYpdVY97DI7njcO\nIoGUGzDQdWiD6CfLsP3eliKXroHa89R60RhWkx/fTAi3qx1rdMwvtw2VR3fbPc+O\nKmfwwErZZ5xpS6ihB0/m3mUq8caE/GAiiJoF8xrUEcDAYtkZnB/Opln6nKKzbrAF\nO4oLCGAjpNCljMfHeM5uG0s=\n-----END PRIVATE KEY-----\n";
    config.signer.tokens.scope = "https://www.googleapis.com/auth/cloud-platform, https://www.googleapis.com/auth/userinfo.email";
    config.signer.expiredSeconds = 3600;
    config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

    /* Create token */
    Firebase.begin(&config, &auth);

    esp_task_wdt_init(watchDogFeedInterval, true);
    esp_task_wdt_reset();
    esp_task_wdt_add(NULL);

    miThermometer.begin();

    lastResetWas = millis();
  }
}

void loop()
{
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
  if (Firebase.ready())
  {
    digitalWrite(LED_BUILTIN, HIGH); // turn the LED on

    scanAndUploadData();

    esp_task_wdt_reset(); // feed the dog

    Serial.printf("Going to sleep for: %d seconds.\n", sleeptime);
    delay(sleeptime * 1000);
  }
  else
  {
    delay(200);
  }

  // reboot
  uint32_t now = millis();
  if (now >= lastResetWas + resetAfterMillis)
  {
    lastResetWas = now;
    resetESP();
  }
}

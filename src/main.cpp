#include <Arduino.h>
#include <WiFiManager.h>
#include <esp_task_wdt.h>
#include "ATC_MiThermometer.h"
#include <ArduinoJson.h>

#include <Preferences.h>

#include <addons/TokenHelper.h>

#include "utils.h"
#include "config.h"
#include "jsonutils.h"
#include "publisher.h"
#include "UpdateGateWayId.h"

FirebaseAuth auth;
FirebaseConfig config;
ATC_MiThermometer miThermometer;

const int scanTime = 10; // BLE scan time in seconds
const int sleeptime = 2;
const int watchDogFeedInterval = scanTime + sleeptime + 10;

uint32_t resetAfterMillis = 1 * 60 * 60 * 1000;
uint32_t lastResetWas;

void scanAndUploadData()
{
  log_i("Start scanning...");

  // Get sensor data
  miThermometer.resetData();
  miThermometer.getData(scanTime);

  String sensorDataAsJson = buildJson(miThermometer.data);
  miThermometer.clearScanResults(); // Delete results fromBLEScan buffer to release memory

  log_i("%s", sensorDataAsJson.c_str());

  send(sensorDataAsJson, String(Firebase.getToken()));
}

void setupWatchDog()
{
  esp_task_wdt_init(watchDogFeedInterval, true);
  esp_task_wdt_reset();
  esp_task_wdt_add(NULL);
}

void setup()
{
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // turn the LED off

  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  WiFiManager wm;
  // wm.resetSettings();
  wm.setConfigPortalTimeout(180);

  if (!wm.autoConnect("SmartHeatingAP"))
  {
    resetESP();
  }
  else
  {
    log_i("Wifi connected successfully!");

    digitalWrite(LED_BUILTIN, HIGH); // turn the Blue LED on (indicate WiFi is on)

    syncTime();

    /* Firebase configuration */
    config.service_account.data.client_email = "esp-temp-sensor@smart-heating-1.iam.gserviceaccount.com";
    config.service_account.data.project_id = "smart-heating-1";
    config.service_account.data.private_key = "-----BEGIN PRIVATE KEY-----\nMIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQDBU44J69xWE3IU\nmrh0Qon7z4gSWPqrjkKJFIyfLuHaSOg/F/WhSWTJLph+QfmRK5jZJ9E2FVIDWYyZ\nzWIK7tyJ/MWclv50csbGJxe6zcL6Gve5qZG96hDN2+IM4h1wEbrBMsJbPspwJwQF\n/Y4j41NoWIxf2YWS0BFbEBRaQckmPi3W1XhtMz38K/1C7E0uyFJ70B+EK1/DUGJc\n0t1drRdDOT5Zqse94Y+oQiU6EmginAXYQuEgUKWrDMktT7W9HoCKNFB94p/2+0Wu\nJUVGjtPsgu0ZyHr5rGEIXq5EZZGb4x8Xj6Yy8BWgx5TWfs4EHc32zOvMi2pMILJR\nlWgKvQUdAgMBAAECggEAHXglILIJZj2UmDt1fpNeATqDXXiezWPkFI1DWkiFOQcA\nEA4Q7UMIz/oAWM6pwk1JAWEmbP4XAFe4LQG3vj7m80nKvbHum8WbMz1mSp1u6T0P\nIggNJUv2v8qSzK/zGNA7DZQ1OD98EFiCLn+WGTZf0noc+8dmqxh4bM8oqxF4HhH8\nJHvlpiuPXvO//QxccGxnfRNdZCTZFfqdBMPVy/JCwuMBJwUWjb40KCLmOquMnxO2\nof35jhzLZyZPqnoDc+9Urz3itJ0n6LlDvUHHdnUXTtlsJiDfgmUBHEp4GfRvuzDG\n/n/vNEyERmFtlUXd7dj71GxXlroejuSDiYw90a/EwQKBgQD4Hhv09+7KNon4L8fJ\naJKowBl9VlCrwD4ogNIXhHuc9UpENl88Z/lvweEuS1wFRnTXlk4aslnqCf6wGk0n\ngli9FnrgnAIQV7cO6CoFXE40cr3A8qGW1gvXRNeycT82Lbre9p3VDEc7qlUYwtgS\nAWApi+Rro7H2doEZ/EjiMX7zFQKBgQDHd9btpiFOPjLHzx+OdzW3bkA7LTuwtfId\n/qCCM83TwnwfPj9BQZeCSym01FLf2IFxC6pesdzdC964jEdWKXsTFPI5iw+lxAjn\nhotzxncBrLH6mr+hEe6oYPtdf7Lyh0794RztSC9fCghlQIKfa3mWv6xh8feBCAb1\nMuo9Ojhr6QKBgAyhO7sUg8s3S36esATIle6RGLsQkbqsZn2ZURhxXfl3Yvhl7CMf\nB8twiw3YOC7sjzYKKJ+jRIBtUdGVBShlsi6t3kNgrZo3XNIdb0YmlLSGwrH3p9IN\nwzyJ/JDQwu73FPQUiaQ2o8mdugcwo98GwuZagJ4aDw0Eqz2vYeZhrpyhAoGAMpNP\nOriZz0X0CDebL7tIMndQ7/A7J0Yq2rIaLolGEgBJCn70+O3Rpaa7L296h+lUgL5N\nd28vqhh9Y1umJtGjtCXmePKqiARZoaK6ryNOP21zPzCHkCaE1cqpUX60d8wZGkQ+\n2mcoI7fegxXFSPiuPqvg4IDCmcPYR8meHOIrNikCgYEAgvId6EYpdVY97DI7njcO\nIoGUGzDQdWiD6CfLsP3eliKXroHa89R60RhWkx/fTAi3qx1rdMwvtw2VR3fbPc+O\nKmfwwErZZ5xpS6ihB0/m3mUq8caE/GAiiJoF8xrUEcDAYtkZnB/Opln6nKKzbrAF\nO4oLCGAjpNCljMfHeM5uG0s=\n-----END PRIVATE KEY-----\n";
    config.signer.tokens.scope = "https://www.googleapis.com/auth/cloud-platform, https://www.googleapis.com/auth/userinfo.email";
    config.token_status_callback = tokenStatusCallback;
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

    //wm.startConfigPortal();

    setupWatchDog();

    miThermometer.begin();

    Preferences preferences;
    preferences.clear();
    preferences.begin("my-app", false);

    bool paired = preferences.getBool("paired", false);
    if (!paired)
    {
      callFunction("Aj19");
      preferences.putBool("paired", true);
    }
    else
    {
      log_i("Was paired");
    }

    preferences.end();

    lastResetWas = millis();
  }
}

void loop()
{
  esp_task_wdt_reset(); // feed the dog

  syncTime();
  if (Firebase.ready())
  {
    scanAndUploadData();
    delay(sleeptime * 1000);
  }
  else
  {
    log_d("Token not ready...");
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

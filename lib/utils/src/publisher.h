#include <HTTPClient.h>
#include <base64.h>
#include <ArduinoJson.h>

void send(String jsonString, String access_token)
{
  String request = "https://pubsub.googleapis.com/v1/projects/smart-heating-1/topics/temperature-sensor:publish";
  HTTPClient http;

  // Start the request

  http.begin(request);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + access_token);

  // Base64 encode data
  String encoded = base64::encode(jsonString);

  int statusCode = http.POST("{\"messages\":[{\"attributes\":{\"gatewayId\": \"" + WiFi.macAddress() + "\"},\"data\": \"" + encoded + "\"}]}");
  
  if (statusCode != 200){
    resetESP();
  }
  
  // Response from server
  String response = http.getString();
  Serial.printf(response.c_str());

  // Parse JSON, read error if any
  DynamicJsonDocument doc(2048);
  DeserializationError error = deserializeJson(doc, response);
  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());

    resetESP();

    return;
  }
  // Print parsed value on Serial Monitor
  Serial.println(doc["messageIds"].as<char *>());
  // Close connection

  http.end();
}

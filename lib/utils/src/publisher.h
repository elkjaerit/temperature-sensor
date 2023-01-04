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

  log_i("Status code %i", statusCode);

  if (statusCode != 200)
  {
    ESP.restart();
  }

  http.end(); // Close connection
}

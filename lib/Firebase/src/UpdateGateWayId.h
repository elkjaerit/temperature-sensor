#include <Arduino.h>
#include <Firebase_ESP_Client.h>

#define PROJECT_LOCATION "europe-west1"

#define FIREBASE_PROJECT_ID "smart-heating-1"

// Define Firebase Data object
FirebaseData fbdo;

boolean callFunction(String key)
{
    FirebaseJson payload;
    
    log_i("Call the Cloud Function... ");
    log_d(Firebase.getToken());
    
    payload.set("id", key);
    payload.set("gatewayId", WiFi.macAddress());
    Serial.println("Mac: " + WiFi.macAddress());
    if (Firebase.Functions.callFunction(&fbdo, FIREBASE_PROJECT_ID, PROJECT_LOCATION, "api-http-linkTempSensor", payload.raw()))
    {
        FirebaseJson js;
        FirebaseJsonData result;

        js.setJsonData(fbdo.payload());        
        js.get(result, "result");

        return result.success;
    }

    else
    {
        Serial.println(fbdo.errorReason());
        return false;
    }
}

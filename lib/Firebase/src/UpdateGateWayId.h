#include <Arduino.h>
#include <Firebase_ESP_Client.h>

#define PROJECT_LOCATION "europe-west1"

#define FIREBASE_PROJECT_ID "smart-heating-1"
#define FIREBASE_CLIENT_EMAIL "esp-temp-sensor@smart-heating-1.iam.gserviceaccount.com"
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\nMIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQDBU44J69xWE3IU\nmrh0Qon7z4gSWPqrjkKJFIyfLuHaSOg/F/WhSWTJLph+QfmRK5jZJ9E2FVIDWYyZ\nzWIK7tyJ/MWclv50csbGJxe6zcL6Gve5qZG96hDN2+IM4h1wEbrBMsJbPspwJwQF\n/Y4j41NoWIxf2YWS0BFbEBRaQckmPi3W1XhtMz38K/1C7E0uyFJ70B+EK1/DUGJc\n0t1drRdDOT5Zqse94Y+oQiU6EmginAXYQuEgUKWrDMktT7W9HoCKNFB94p/2+0Wu\nJUVGjtPsgu0ZyHr5rGEIXq5EZZGb4x8Xj6Yy8BWgx5TWfs4EHc32zOvMi2pMILJR\nlWgKvQUdAgMBAAECggEAHXglILIJZj2UmDt1fpNeATqDXXiezWPkFI1DWkiFOQcA\nEA4Q7UMIz/oAWM6pwk1JAWEmbP4XAFe4LQG3vj7m80nKvbHum8WbMz1mSp1u6T0P\nIggNJUv2v8qSzK/zGNA7DZQ1OD98EFiCLn+WGTZf0noc+8dmqxh4bM8oqxF4HhH8\nJHvlpiuPXvO//QxccGxnfRNdZCTZFfqdBMPVy/JCwuMBJwUWjb40KCLmOquMnxO2\nof35jhzLZyZPqnoDc+9Urz3itJ0n6LlDvUHHdnUXTtlsJiDfgmUBHEp4GfRvuzDG\n/n/vNEyERmFtlUXd7dj71GxXlroejuSDiYw90a/EwQKBgQD4Hhv09+7KNon4L8fJ\naJKowBl9VlCrwD4ogNIXhHuc9UpENl88Z/lvweEuS1wFRnTXlk4aslnqCf6wGk0n\ngli9FnrgnAIQV7cO6CoFXE40cr3A8qGW1gvXRNeycT82Lbre9p3VDEc7qlUYwtgS\nAWApi+Rro7H2doEZ/EjiMX7zFQKBgQDHd9btpiFOPjLHzx+OdzW3bkA7LTuwtfId\n/qCCM83TwnwfPj9BQZeCSym01FLf2IFxC6pesdzdC964jEdWKXsTFPI5iw+lxAjn\nhotzxncBrLH6mr+hEe6oYPtdf7Lyh0794RztSC9fCghlQIKfa3mWv6xh8feBCAb1\nMuo9Ojhr6QKBgAyhO7sUg8s3S36esATIle6RGLsQkbqsZn2ZURhxXfl3Yvhl7CMf\nB8twiw3YOC7sjzYKKJ+jRIBtUdGVBShlsi6t3kNgrZo3XNIdb0YmlLSGwrH3p9IN\nwzyJ/JDQwu73FPQUiaQ2o8mdugcwo98GwuZagJ4aDw0Eqz2vYeZhrpyhAoGAMpNP\nOriZz0X0CDebL7tIMndQ7/A7J0Yq2rIaLolGEgBJCn70+O3Rpaa7L296h+lUgL5N\nd28vqhh9Y1umJtGjtCXmePKqiARZoaK6ryNOP21zPzCHkCaE1cqpUX60d8wZGkQ+\n2mcoI7fegxXFSPiuPqvg4IDCmcPYR8meHOIrNikCgYEAgvId6EYpdVY97DI7njcO\nIoGUGzDQdWiD6CfLsP3eliKXroHa89R60RhWkx/fTAi3qx1rdMwvtw2VR3fbPc+O\nKmfwwErZZ5xpS6ihB0/m3mUq8caE/GAiiJoF8xrUEcDAYtkZnB/Opln6nKKzbrAF\nO4oLCGAjpNCljMfHeM5uG0s=\n-----END PRIVATE KEY-----\n";

// Define Firebase Data object
FirebaseData fbdo;

boolean callFunction(String key)
{
    Serial.println(Firebase.getToken());
    Serial.print("Call the Cloud Function... ");

    FirebaseJson payload;
    payload.set("id", key);
    payload.set("gatewayId", WiFi.macAddress());
    Serial.println("Mac: " + WiFi.macAddress());
    if (Firebase.Functions.callFunction(&fbdo, FIREBASE_PROJECT_ID, PROJECT_LOCATION, "api-http-linkTempSensor", payload.raw()))
    {

        FirebaseJson js;
        js.setJsonData(fbdo.payload());

        FirebaseJsonData result;
        js.get(result, "result");
        return result.success;
    }

    else
    {
        Serial.println(fbdo.errorReason());
        return false;
    }
}

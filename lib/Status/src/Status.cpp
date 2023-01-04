#include "Status.h"
#include "Arduino.h"
#include <SPIFFS.h>
#include <string>

Status::Status(std::string ss)
{
    statusFileName = ss;
    if (SPIFFS.begin(true))
    {
        Serial.println("Mounted file system");
        if (SPIFFS.exists(statusFileName.c_str()))
        {
            File configFile = SPIFFS.open(statusFileName.c_str(), "r");
            if (json.readFrom(configFile))
            {
                json.toString(Serial, true /* prettify option */);
                Serial.println("");
            }

            configFile.close();
        }
    }
}

void Status::persist()
{
    File file = SPIFFS.open(statusFileName.c_str(), "w");
    json.toString(file);

    // Due to bugs in SPIFFS print
    // https://github.com/esp8266/Arduino/issues/8372
    // append new line to separate each JSON data (recommended)
    //file.println();

    file.close();

    Serial.println("Persisted file");
    json.toString(Serial, true /* prettify option */);
    Serial.println("");
}

void Status::setUpdated()
{
    json.set("apiTokenUpdated", true);
    persist();
}

bool Status::isUpdated()
{
    FirebaseJsonData result;
    json.get(result, "apiTokenUpdated");

    if (result.success)
    {
        if (result.typeNum == FirebaseJson::JSON_BOOL)
            return result.to<bool>();
    }

    return false;
}

#include <ArduinoJson.h>

String buildJson(std::vector<MiThData_t> data)
{

    const size_t CAPACITY = JSON_ARRAY_SIZE(128);
    StaticJsonDocument<CAPACITY> doc1;

    // create an empty array
    JsonArray sensor = doc1.to<JsonArray>();

    for (int i = 0; i < data.size(); i++)
    {
        if (data[i].valid)
        {

            DynamicJsonDocument doc(256);

            String name = data[i].name.c_str();
            name.toUpperCase();

            unsigned long timestamp = time(nullptr);
            char *macAddress = const_cast<char *>(name.c_str());

            // Set value that is always present
            doc["sensorId"] = macAddress;
            doc["rssi"] = data[i].rssi;
            doc["timestamp"] = timestamp;

            // Temperature, humidity and batt is optional
            if (data[i].temperature > 0)
            {
                doc["temperature"] = data[i].temperature / 100.0;
            }

            if (data[i].humidity > 0)
            {
                doc["humidity"] = data[i].humidity / 100.0;
            }

            if (data[i].batt_level > 0)
            {
                doc["batt"] = data[i].batt_level;
            }

            sensor.add(doc);
        }
    }
    String jsonAsString;
    serializeJson(doc1, jsonAsString);
    return jsonAsString;
}
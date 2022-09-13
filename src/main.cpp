#include <Arduino.h>
#include "ATC_MiThermometer.h"

const int scanTime = 5; // BLE scan time in seconds

// List of known sensors' BLE addresses
std::vector<std::string> knownBLEAddresses = {"a4:c1:38:b8:1f:7f", "a4:c1:38:bf:e1:bc"};

ATC_MiThermometer miThermometer(knownBLEAddresses);


void setup() {
    Serial.begin(115200);
    
    // Initialization
    miThermometer.begin();
}

void loop() {
    // Set sensor data invalid
    miThermometer.resetData();
    
    // Get sensor data - run BLE scan for <scanTime>
    unsigned found = miThermometer.getData(scanTime);

    for (int i=0; i < miThermometer.data.size(); i++) {  
        if (miThermometer.data[i].valid) {
            Serial.println();
            Serial.printf("Sensor %d: %s\n", i, knownBLEAddresses[i].c_str());
            Serial.printf("%.2f°C\n", miThermometer.data[i].temperature/100.0);
            Serial.printf("%.2f%%\n", miThermometer.data[i].humidity/100.0);
            Serial.printf("%.3fV\n",  miThermometer.data[i].batt_voltage/1000.0);
            Serial.printf("%d%%\n",   miThermometer.data[i].batt_level);
            Serial.printf("%ddBm\n",  miThermometer.data[i].rssi);
            Serial.println();
         }
    }
    Serial.print("Devices found: ");
    Serial.println(found);
    Serial.println();

    // Delete results fromBLEScan buffer to release memory
    miThermometer.clearScanResults();
    delay(5000);
}
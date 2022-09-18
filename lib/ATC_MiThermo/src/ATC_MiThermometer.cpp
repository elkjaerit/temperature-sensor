#include "ATC_MiThermometer.h"

/*!
 * \class MyAdvertisedDeviceCallbacks
 *
 * \brief Callback for advertised device found during scan
 */
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
        
    }     
};

// Set up BLE scanning
void ATC_MiThermometer::begin(void)
{
    BLEDevice::init("");
    _pBLEScan = BLEDevice::getScan(); // create new scan
    _pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    _pBLEScan->setActiveScan(false); // active scan uses more power, but get results faster
    _pBLEScan->setInterval(100);
    _pBLEScan->setWindow(99); // less or equal setInterval value
}

// Get sensor data by running BLE device scan
unsigned ATC_MiThermometer::getData(uint32_t duration)
{
    BLEScanResults foundDevices = _pBLEScan->start(duration, false /* is_continue */);

    // Initialize data vector
    std::vector<BLEAdvertisedDevice> supportedDevices = getSupportedDevices(foundDevices);
    DEBUG_PRINTF("Supported devices found: %d\n", supportedDevices.size());

    data.resize(supportedDevices.size());

    Serial.printf("Iteration: %d \n", supportedDevices.size());

    for (unsigned i = 0; i < supportedDevices.size(); i++)
    {
        DEBUG_PRINT("Found: ");
        DEBUG_PRINTLN(supportedDevices[i].getAddress().toString().c_str());
        // Skip devices with wrong ServiceDataUUID
        if (BLEUUID((uint16_t)0x181a).equals(supportedDevices[i].getServiceDataUUID()))
        {

            data[i].valid = true;
            data[i].name = supportedDevices[i].getAddress().toString();

            // Temperature
            int temp_msb = supportedDevices[i].getServiceData().c_str()[7];
            int temp_lsb = supportedDevices[i].getServiceData().c_str()[6];
            data[i].temperature = (temp_msb << 8) | temp_lsb;

            // Humidity
            int hum_msb = supportedDevices[i].getServiceData().c_str()[9];
            int hum_lsb = supportedDevices[i].getServiceData().c_str()[8];
            data[i].humidity = (hum_msb << 8) | hum_lsb;

            // Battery voltage
            int volt_msb = supportedDevices[i].getServiceData().c_str()[11];
            int volt_lsb = supportedDevices[i].getServiceData().c_str()[10];
            data[i].batt_voltage = (volt_msb << 8) | volt_lsb;

            // Battery state [%]
            data[i].batt_level = supportedDevices[i].getServiceData().c_str()[12];

            // Received Signal Strength Indicator [dBm]
            data[i].rssi = supportedDevices[i].getRSSI();
        }
        else
        {
            data[i].valid = false;

            std::string serviceData = supportedDevices[i].getServiceData();
            int serviceDataLength = serviceData.length();
            String returnedString = "";
            for (int i = 0; i < serviceDataLength; i++)
            {
                int a = serviceData[i];
                if (a < 16)
                {
                    returnedString = returnedString + "0";
                }
                returnedString = returnedString + String(a, HEX);
            }

            char service_data[returnedString.length() + 1];
            returnedString.toCharArray(service_data, returnedString.length() + 1);
            service_data[returnedString.length()] = '\0';

            int data_length = 0;
            int offset = 0;

            switch (service_data[27 + offset])
            {
            case '1':
            case '2':
            case '3':
            case '4':
                data_length = ((service_data[27 + offset] - '0') * 2);
                DEBUG_PRINTF("Valid data_length: %d\n", data_length);
                break;
            default:
                //   Log.trace(F("Invalid data_length, not enriching the device data" CR));
                continue;
            }

            double value = 9999;
            value = value_from_service_data(service_data, 28 + offset, data_length);

            switch (service_data[23 + offset])
            {
            case 'a':
                data[i].name = supportedDevices[i].getAddress().toString();

                data[i].batt_level = value;
                data[i].rssi = supportedDevices[i].getRSSI();
                data[i].valid = true;

                // sendBattery(macAddress, (double)value, advertisedDevice.getRSSI(), timestamp);
                break;
            case 'd':
                // humidity
                {
                    data[i].name = supportedDevices[i].getAddress().toString();

                    value = value_from_service_data(service_data, 28 + offset, 4);
                    data[i].temperature = (double)value / 10 * 100;

                    value = value_from_service_data(service_data, 32 + offset, 4);
                    data[i].humidity = (double)value / 10 * 100;

                    data[i].rssi = supportedDevices[i].getRSSI();
                    data[i].valid = true;

                    // sendTempAndHumidity(macAddress, temperature, humidity, advertisedDevice.getRSSI(), timestamp);
                    break;
                }
            default:
                continue;
            }
        }
    }
    return supportedDevices.size();
}

double ATC_MiThermometer::value_from_service_data(const char *service_data, int offset, int data_length)
{
    char rev_data[data_length + 1];
    char data[data_length + 1];
    memcpy(rev_data, &service_data[offset], data_length);
    rev_data[data_length] = '\0';

    // reverse data order
    revert_hex_data(rev_data, data, data_length + 1);
    double value = strtol(data, NULL, 16);
    if (value > 65000 && data_length <= 4)
        value = value - 65535;
    return value;
}

void ATC_MiThermometer::revert_hex_data(char *in, char *out, int l)
{
    // reverting array 2 by 2 to get the data in good order
    int i = l - 2, j = 0;
    while (i != -2)
    {
        if (i % 2 == 0)
            out[j] = in[i + 1];
        else
            out[j] = in[i - 1];
        j++;
        i--;
    }
    out[l - 1] = '\0';
}

std::vector<BLEAdvertisedDevice> ATC_MiThermometer::getSupportedDevices(BLEScanResults devices)
{
    std::vector<BLEAdvertisedDevice> result;
    for (unsigned i = 0; i < devices.getCount(); i++)
    {
        if (BLEUUID((uint16_t)0x181a).equals(devices.getDevice(i).getServiceDataUUID()))
        {
            result.push_back(devices.getDevice(i));
        }
        else if (BLEUUID((uint16_t)0xfe95).equals(devices.getDevice(i).getServiceDataUUID()))
        {
            result.push_back(devices.getDevice(i));
        }
        else
        {
            // Unsupported device
        }
    }
    return result;
}

// Set all array members invalid
void ATC_MiThermometer::resetData(void)
{
    for (int i = 0; i < data.size(); i++)
    {
        data[i].valid = false;
        data[i].temperature = 0;
        data[i].batt_level = 0;
        data[i].humidity = 0;
        data[i].batt_voltage = 0;
    }
}
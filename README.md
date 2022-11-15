
# Simple setup guide

Setting up the "Xiaomi Mi Temperature and Humidity Monitor 2" to report data to SmartHeating servers involves 2 steps:

- Setup ESP32
- Upload custom firmware to sensors


## Setup ESP32
1. Upload project with PlatformIO
2. Connect to access point SmartingHeatingAP and visit http://192.168.4.1 to enter credentials
## Upload custom firmware Xiaomi temperature sensor
For better perfomance/stability the sensors must be flashed. 

Open [Telink Flasher for MI Thermostat](https://pvvx.github.io/ATC_MiThermometer/TelinkMiFlasher.html) and follow the steps below:

### ___1. Connect to the device (use filter)___

![Connect](docs/temp-guide1.png)
![Connect](docs/temp-guide2.png)

### ___2. Activate___
![Connect](docs/temp-guide3.png)


### ___3. Upload new firmware___
![Connect](docs/temp-guide4.png)
![Connect](docs/temp-guide5.png)

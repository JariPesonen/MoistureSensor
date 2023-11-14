# MoistureSensor

## Functionality
This code works with I2C moisture sensor and Espressif ESP32-WROOM https://www.espressif.com/en/products/socs/esp32

It will connect to Wifi, read the Chrip moisture sensor value and upload the data to InfluxDB Cloud

## I2C Soil moisture sensor
This code uses I2C soil moisture sensor sold by Catnip electronics.
https://www.tindie.com/products/miceuz/i2c-soil-moisture-sensor/

It's recommended to check the moisture sensor calibration guide to understand how accurate the values are 
and what those are in your environment.

Measure your own capacitance values based on soil you use, location, sensor etc, for example:
- 250: Dry air
- 556: half moist
- 574: fully soaked/wet

## Requirements
### To use this
1) Ensure your environment is setup correctly, so that you can upload and run code on ESP32
2) You have installed the referenced libraries to your IDE, including
- https://github.com/Apollon77/I2CSoilMoistureSensor
3) You have setup InfluxDB cloud
4) Chrip sensor is connected to ESP32 and can read values
5) You have Wifi connection and it's configured correctly and device can connect through Internet

# References
I2C Sensor code examples: https://github.com/Miceuz/i2c-moisture-sensor/blob/master/README.md
Setting up ESP32 development: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/
Recommended: https://platformio.org/platformio-ide
Setting up Espressif ESP32 in PlatformIO IDE: https://docs.platformio.org/en/latest/platforms/espressif32.html
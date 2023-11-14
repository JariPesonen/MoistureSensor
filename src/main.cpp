
#include <WiFiMulti.h>
#include <Arduino.h>
#include <I2CSoilMoistureSensor.h>
#include <Wire.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

WiFiMulti wifiMulti;
I2CSoilMoistureSensor moist_sensor;

/*

This code works with Chrip sensor and Expressif ESP32-WROOM https://www.espressif.com/en/products/socs/esp32
It will connect to Wifi, read the Chrip moisture sensor value and upload the data to InfluxDB Cloud

It's recommended to check the moisture sensor calibration guide to understand how accurate the values are 
and what those are in your environment.

Measure your own capacitance values based on soil you use, location, sensor etc, for example:
250: Dry air
556: half moist
574: fully soaked/wet

To use this:
1) Ensure your environment is setup correctly, so that you can upload and run code on ESP32
2) You have installed the referenced libraries to your IDE
3) You have setup InfluxDB cloud
4) Chrip sensor is connected to ESP32 and can read values
5) You have Wifi connection and it's configured correctly and device can connect through Internet

*/

// WiFi AP SSID
#define WIFI_SSID "YOURWIFISSID"
// WiFi password
#define WIFI_PASSWORD "YOURWIFIPASSWORD"

// You can sign up for free to InfluxDB Cloud to store your data https://cloud2.influxdata.com/signup
// Getting started https://docs.influxdata.com/influxdb/v2/get-started/
// You can find detailed instructions from this page https://github.com/tobiasschuerg/InfluxDB-Client-for-Arduino
// It's also possible to use local installations
// InfluxDB server url, depends on your hosting location and region. E.g. https://eu-central-1-1.aws.cloud2.influxdata.com 
#define INFLUXDB_URL "YOURINFLUXDBURL"
// InfluxDB v2 server or cloud API token (Use: InfluxDB UI -> Data -> API Tokens -> Generate API Token)
#define INFLUXDB_TOKEN "YOURINFLUXDBTOKEN"
// InfluxDB v2 organization id (Use: InfluxDB UI -> User -> About -> Common Ids )
#define INFLUXDB_ORG "YOURINFLUXDBORG"
// InfluxDB v2 bucket name (Use: InfluxDB UI ->  Data -> Buckets)
#define INFLUXDB_BUCKET "moisture_data" 

// Set timezone string according to https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
// Examples:
//  Pacific Time: "PST8PDT"
//  Eastern: "EST5EDT"
//  Japanesse: "JST-9"
//  Central Europe: "CET-1CEST,M3.5.0,M10.5.0/3"
#define TZ_INFO "CET-1CEST,M3.5.0,M10.5.0/3"

// InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

// Data point
Point sensor("moist_sensor");

void setup() {
  Wire.begin();
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());

  moist_sensor.begin(); // reset sensor
  Wire.setClock(40000);
  delay(1000); // give some time to boot up
  Serial.print("I2C Soil Moisture Sensor Address: ");
  Serial.println(moist_sensor.getAddress(),HEX);
  Serial.print("Sensor Firmware version: ");
  Serial.println(moist_sensor.getVersion(),HEX);
  Serial.println();

  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  // Add permanent tags - only once
  sensor.addTag("device", "moist_sensor3");
  sensor.addTag("location", "balcony");

  // Check server connection
  if (client.validateConnection()) {

    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
}

void loop() {
  while (moist_sensor.isBusy()) delay(50); // available since FW 2.3
  sensor.clearFields();
  Serial.print("Soil Moisture Capacitance: ");
  Serial.print(moist_sensor.getCapacitance()); //read capacitance register
  sensor.addField("capacitance", moist_sensor.getCapacitance());
  Serial.print(", Temperature: ");
  Serial.print(moist_sensor.getTemperature()/(float)10); //temperature register
  sensor.addField("temp", moist_sensor.getTemperature()/(float)10);
  moist_sensor.sleep(); // available since FW 2.3
  // Check WiFi connection and reconnect if needed
  if (wifiMulti.run() != WL_CONNECTED) {
    Serial.println("Wifi connection lost");
  }
  // Write point
  if (!client.writePoint(sensor)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }
  Serial.println("Wait 300s");
  delay(300000);
}
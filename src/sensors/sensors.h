#ifndef _SENSORS
#define _SENSORS

#include <DHT.h>

float readBatteryVoltage();
float readTemperatur();
float readHumiditys();

#define DHTPIN 16
#define DHTTYPE DHT11

extern DHT dht; 

#endif

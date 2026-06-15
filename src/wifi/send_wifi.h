#ifndef _WIFI_SEND_H
#define _WIFI_SEND_H

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <iostream>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

bool connectToWifi();
bool send(float temp, float battery, float hum);

#endif

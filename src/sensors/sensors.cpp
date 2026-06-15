#include <Arduino.h>
#include "sensors.h"


DHT dht(DHTPIN, DHTTYPE); 



float readBatteryVoltage() {
  long sum = 0;
  int samples = 20; // Делаем 20 замеров для сглаживания шумов
  
  for (int i = 0; i < samples; i++) {
    sum += analogRead(34);
    delay(1); 
  }
  
  float raw = sum / (float)samples;
   const float calibrationFactor = 1.00; 
  
  return (raw / 4095.0) * 3.3 * 2.0 * calibrationFactor;
}
float readTemperatur(){
  float t = dht.readTemperature();
  return isnan(t) ? -1.0 : t; 
}

float readHumiditys(){
  float h = dht.readHumidity();
  return isnan(h) ? -1.0 : h;
}

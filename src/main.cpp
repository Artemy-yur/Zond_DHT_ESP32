#include <Arduino.h>
#include "sensors/sensors.h"
#include "wifi/send_wifi.h"

RTC_DATA_ATTR int bootCount = 0;

void init(){
  Serial.begin(115200);
  dht.begin();
  bootCount++;
  
  connectToWifi();
  Serial.println("System starts");
}

void print_terminal(float hum, float temp, float volt){
  Serial.printf("DHT: Влажность %.2f%% | Темп %.2f°C | Батарея %.2f\n", hum, temp, volt);
}

void setup() {
  init();
  delay(1000);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Соединение потеряно. Пробуем переподключиться...");
    if (!connectToWifi()) {
      Serial.println("Пропуск шага отправки");
      delay(5000);
      return; 
    }
  }

  float temp = readTemperatur();
  float hum = readHumiditys();
  float batt = readBatteryVoltage();

  print_terminal(hum,temp,batt);

  send(temp, batt, hum);

  Serial.println("------------------------------------");
  
  delay(5000); 
}

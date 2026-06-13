#include <Arduino.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <iostream> 
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define DHTPIN 4
#define DHTTYPE DHT11

const char* ssid = "MTS_GPON_5F42";
const char* password = "dXEUbAeH";

const char* serverUrl = "http://192.168.1.6:80";

RTC_DATA_ATTR int bootCount = 0;
DHT dht(DHTPIN, DHTTYPE);

static float readBatteryVoltage() {
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

static bool connectToWifi(){
  // Если уже подключены, ничего не делаем
  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }

  Serial.print("Подключение к Wi-Fi: ");
  Serial.println(ssid);
  
  // Отключаем старые сессии перед новым стартом
  WiFi.disconnect(true);
  delay(100);
  
  WiFi.begin(ssid, password);

  int attempts = 0;
  // Ждем подключения максимум 15 секунд (60 * 250мс)
  while(WiFi.status() != WL_CONNECTED && attempts < 60){
    delay(250);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nУспешно подключено!");
    Serial.print("IP-адрес ESP32: ");
    Serial.println(WiFi.localIP());
    return true;
  } else {
    Serial.println("\nНе удалось подключиться к Wi-Fi.");
    return false;
  }
}

bool send(float temp, float battery, float hum){
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("Нет подключения к WiFi");
    return false;
  }
  
  StaticJsonDocument<512> doc; 
  doc["device_id"] = "NOLERR_1";
  doc["temperature"] = round(temp * 10) / 10.0;
  doc["hum"] = round(hum * 10) / 10.0;
  doc["battery"] = round(battery * 10) / 10.0;
  doc["rssi"] = WiFi.RSSI();
  
  String JsonString;
  serializeJson(doc, JsonString);
  Serial.println("Отправка JSON: " + JsonString);


  WiFiClient client; 
  HTTPClient http;
  
  http.begin(client, serverUrl); 
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(10000); 
  
  int code = http.POST(JsonString);
  Serial.printf("HTTP Response code: %d\n", code); 
  
  if (code < 0) {
    Serial.printf("Ошибка отправки: %s\n", http.errorToString(code).c_str());
  } else {
    // Если сервер ответил, можно прочитать что он прислал
    String response = http.getString();
    Serial.println("Ответ сервера: " + response);
  }
  
  http.end(); // Обязательно закрываем соединение
  return code == 200;
}

static float readTemperatur(){
  float t = dht.readTemperature();
  return isnan(t) ? -1.0 : t; 
}

static float readHumiditys(){
  float h = dht.readHumidity();
  return isnan(h) ? -1.0 : h;
}


void setup() {
  Serial.begin(115200);
  dht.begin();
  delay(1000);
  bootCount++;
  
  connectToWifi();
  Serial.println("Система запущена");
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Соединение потеряно. Пробуем переподключиться...");
    if (!connectToWifi()) {
      Serial.println("Пропуск шага отправки, ждем следующего цикла...");
      delay(5000);
      return; 
    }
  }

  float temp = readTemperatur();
  float hum = readHumiditys();
  float batt = readBatteryVoltage();

  Serial.printf("DHT: Влажность %.2f%% | Темп %.2f°C | Батарея %.2f\n", hum, temp, batt);
  
  send(temp, batt, hum);

  Serial.println("------------------------------------");
  delay(5000); 
}

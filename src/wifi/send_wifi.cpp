#include "send_wifi.h"


const char* ssid = "MTS_GPON_5F42";
const char* password = "dXEUbAeH"; 
const char* serverUrl = "http://192.168.1.7:8080";


bool connectToWifi(){
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
  
  http.end(); 
  return code == 200;
}


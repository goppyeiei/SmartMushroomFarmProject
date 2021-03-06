#include <Arduino_JSON.h>

#include <dummy.h>

#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = ""; //Wifi name  
const char* password = ""; //Wifi password
int user_id = ; //User ID
int farm_id = ; //Farm ID
int fan = ; //Arduino port
int fog = ; // Arduino port
String fan_status;
String fog_status;
uint32_t notConnectedCounter = 0;
unsigned long lastMillis;

void setup () {

  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {

    delay(100);
    notConnectedCounter++;
    if(notConnectedCounter > 150) { // Reset board if not connected after 5s
        Serial.println("Resetting due to Wifi not connecting...");
        ESP.restart();
    }
  }

  Serial.println("Connected");

  pinMode(fan, OUTPUT);
  pinMode(fog, OUTPUT);

}

void loop() {
  if (millis() - lastMillis >= 3600000) 
  {
   lastMillis = millis();  
   statistic();
  }
  if (millis() >= 86400000){
    ESP.restart();
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    http.begin(client, "http://139.59.249.192/check-iot/" + String(farm_id) );
    int dataa = http.GET();
    String iot_status = http.getString();
    Serial.println(iot_status);
    http.end();
    fan_status = iot_status[0];
    fog_status = iot_status[3]; 
    http.begin(client, "http://139.59.249.192/check-condition/" + String(farm_id) + "/" + String(fan_status) + "/" + String(fog_status) );
    http.GET();
    http.end();
    delay(1000);
  } else {
    Serial.println("Resetting due to Wifi not connecting...");
    ESP.restart();
  };

  if (fan_status == "1") {
    digitalWrite(fan,LOW);
    Serial.println("Fan on");
  } else {
    digitalWrite(fan,HIGH);
    Serial.println("Fan off");
  }
  
  if (fog_status == "1") {
    digitalWrite(fog,LOW);
    Serial.println("Fog on");
  } else {
    digitalWrite(fog,HIGH);
    Serial.println("Fog off");
  }

  
}

void statistic(){
    WiFiClient client;
    HTTPClient http;
    http.begin(client, "http://139.59.249.192/statistic/" + String(farm_id) );
    http.GET();
    http.end();
    Serial.println("Statistic");
}

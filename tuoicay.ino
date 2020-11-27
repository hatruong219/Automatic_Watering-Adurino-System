#include <DS3231.h>
#include <Wire.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
const uint8_t fingerprint[20] = {0xEE,0xAA,0x58,0x6D,0x4F,0x1F,0x42,0xF4,0x18,0x5B,0x7F,0xB0,0xF2,0x0A,0x4C,0xDD,0x97,0x47,0x7D,0x99};

ESP8266WiFiMulti WiFiMulti;
StaticJsonDocument<1024> doc;

DS3231 Clock;
bool Century=false;
bool h12;
bool PM;
int const T_RELAY_PIN = D2;
int const LED_BLUE_PIN = D7;
int const LED_YEWLOW_PIN = D6;
int const LED_RED_PIN = D5;
int const LED_WANNING_PIN = D8;
void setup() {
  Wire.begin();
  // Start the serial interface
  Serial.begin(9600);
  Clock.setHour(8);
  Clock.setMinute(22);
  Clock.setSecond(0);
  pinMode(T_RELAY_PIN, OUTPUT);
  pinMode(LED_BLUE_PIN, OUTPUT);
  pinMode(LED_YEWLOW_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_WANNING_PIN, OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("Redmi", "truongabc");
}
void loop() {
  Serial.print(Clock.getHour(h12, PM), DEC);
  Serial.print(" : ");
  Serial.print(Clock.getMinute(), DEC);
  Serial.print(" : ");
  Serial.print(Clock.getSecond(), DEC);
  Serial.print('\n');
  delay(10000);
  //--------------------------------------------
  digitalWrite(T_RELAY_PIN, LOW);
  digitalWrite(LED_BLUE_PIN, LOW);
  digitalWrite(LED_YEWLOW_PIN, LOW);
  digitalWrite(LED_WANNING_PIN, LOW);
  digitalWrite(LED_RED_PIN, LOW);
  dongho();
}

//hàm lấy thời gian
void dongho(){
  if(Clock.getHour(h12, PM) == 8 && Clock.getMinute() == 23 && Clock.getSecond()== 0){
      laynhietdo(); 
  }
  else{
      digitalWrite(T_RELAY_PIN, LOW);
      digitalWrite(LED_BLUE_PIN, LOW);
      digitalWrite(LED_YEWLOW_PIN, LOW);
      digitalWrite(LED_RED_PIN, LOW);
    }
}


//hàm lấy nhiệt dộ
void laynhietdo(){
  int temp = 5;
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    client->setFingerprint(fingerprint);

    HTTPClient https;

    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, "https://api.openweathermap.org/data/2.5/weather?q=danang&units=metric&appid=17ea2ba52f735b16b100dab55d1ee6a8")) {  // HTTPS

      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      int httpCode = https.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          deserializeJson(doc, payload);
          JsonObject root = doc.as<JsonObject>();
          JsonObject main = root["main"];
          temp = main["temp"];
          Serial.printf("nhiet do: ");
          Serial.println(temp);
          
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }

      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }
  else{
    digitalWrite(LED_WANNING_PIN, 1);
    delay(10000);
  }
  bomnuoc(temp);
}

//Hàm thực hiện bơm
void bomnuoc(int nhietdo){
      Serial.println(nhietdo);
      if(nhietdo < 30 && nhietdo > 10){
        digitalWrite(T_RELAY_PIN, 1);
        digitalWrite(LED_YEWLOW_PIN, 1);
        Serial.println("Bơm 6s");
        delay(6000);
      }else if(nhietdo >= 30){
        digitalWrite(T_RELAY_PIN, 1);
        digitalWrite(LED_BLUE_PIN, 1);
        Serial.println("Bơm 10s");
        delay(10000);
      }
      else{
        digitalWrite(T_RELAY_PIN, 1);
        digitalWrite(LED_RED_PIN, 1);
        Serial.println("Bơm 2s");
        delay(2000);
      }
}

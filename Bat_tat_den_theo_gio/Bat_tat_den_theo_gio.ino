#include <DS3231.h>
int Relay = 6;
DS3231  rtc(SDA, SCL);
Time t;
const int gio_bat = 8;
const int phut_bat = 0;

void setup() {
  Serial.begin(115200);
  rtc.begin();
  pinMode(Relay, OUTPUT);
  digitalWrite(Relay, LOW);
}

void loop() {
  t = rtc.getTime();
  Serial.print(t.hour);
  Serial.print(" hour(s), ");
  Serial.print(t.min);
  Serial.print(" minute(s)");
  Serial.print(t.sec);
  Serial.print(" sec(s)");
  Serial.println(" ");
  delay (1000);
  
  if(t.hour == gio_bat && t.min == phut_bat){
    digitalWrite(Relay,HIGH);
    }
}

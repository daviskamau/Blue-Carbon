#include <LowPower.h>
#include <RTClib.h>
#include <Wire.h>

RTC_DS3231 rtc;

#define SUNDAY    0
#define MONDAY    1
#define TUESDAY   2
#define WEDNESDAY 3
#define THURSDAY  4
#define FRIDAY    5
#define SATURDAY  6

char daysOfTheWeek[7][12] = {
  "Sunday",
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday"
};

const int RELAY_1 = A0;
const int RELAY_2 = A1;

uint8_t DAY = TUESDAY ;
uint8_t START_HH_ODD_WEEK = 12; 
uint8_t START_HH_EVEN_WEEK = 0; 
uint8_t START_MM = 0; 

void setup() {
  Serial.begin(9600);

  if (!rtc.begin()) {
    Serial.println("[ERROR] Couldn't find RTC");
    while (1);
  }

  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
}

void loop() {
  DateTime now = rtc.now();
  int week = (now.day() + 6) / 7; 
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(" - Week:");
  Serial.print(week);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
  
  Serial.print("Pumps are set to run on ");
  Serial.print(daysOfTheWeek[DAY]);
  Serial.print(", at ");
  if (week == 1 || week == 3 || week == 5) {
    Serial.print(START_HH_ODD_WEEK);
    Serial.print(':');
    Serial.print(START_MM);
  } else if (week == 2 || week == 4) {
    Serial.print(START_HH_EVEN_WEEK);
    Serial.print(':');
    Serial.print(START_MM);
  }
  Serial.println(".");

  delay(1000);
  if (now.dayOfTheWeek() == DAY) {
    if (week == 1 || week == 3 || week == 5) {
      if (now.hour() == START_HH_ODD_WEEK && now.minute() >= START_MM){
        unsigned long startTime = millis();
        while (millis() - startTime < 3600000) {
          digitalWrite(RELAY_1, HIGH);
          delay(30000);
          digitalWrite(RELAY_1, LOW);
          digitalWrite(RELAY_2, HIGH);
          delay(30000);
          digitalWrite(RELAY_2, LOW);
          }
          digitalWrite(RELAY_1, LOW);
          digitalWrite(RELAY_2, LOW);
       } else {
      LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
      }
   } else if (week == 2 || week == 4) {
      if (now.hour() == START_HH_EVEN_WEEK && now.minute() >= START_MM){
        unsigned long startTime = millis();
        while (millis() - startTime < 3600000) {
          digitalWrite(RELAY_1, HIGH);
          delay(30000);
          digitalWrite(RELAY_1, LOW);
          digitalWrite(RELAY_2, HIGH);
          delay(30000);
          digitalWrite(RELAY_2, LOW);
          }
          digitalWrite(RELAY_1, LOW);
          digitalWrite(RELAY_2, LOW);
      } else {
        LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
      }
    } else {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    }
  } else {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }
}

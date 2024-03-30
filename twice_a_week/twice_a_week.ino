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

uint8_t DAY_1    = TUESDAY;
uint8_t DAY_2    = FRIDAY;
uint8_t START_HH = 12; 
uint8_t START_MM = 45; 

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

void setup() {
  
  Serial.begin(9600);

  if (! rtc.begin()) {
    Serial.println("[ERROR] Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("[ERROR] RTC lost power, please set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
}

void vacuum_air() {
  digitalWrite(RELAY_1, HIGH);
}

void pump_air() {
  digitalWrite(RELAY_2, HIGH);
}

void stop_pump() {
  digitalWrite(RELAY_1, LOW);
  digitalWrite(RELAY_2, LOW);
}

void loop() {
  
  DateTime now = rtc.now();
  
  Serial.print(now.year(), DEC);
  Serial.print('/');
  
  Serial.print(now.month(), DEC);
  Serial.print('/');
  
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  
  Serial.print(now.second(), DEC);
  Serial.println();

  Serial.print("Pumps are set to run on ");
  Serial.print(daysOfTheWeek[DAY_1]);
  Serial.print(" and ");
  Serial.print(daysOfTheWeek[DAY_2]);
  Serial.print(", at ");
  Serial.print(START_HH);
  Serial.print(":");
  Serial.print(START_MM);
  Serial.println(".");

  delay(1000);

  if (now.dayOfTheWeek() == DAY_1 || now.dayOfTheWeek() == DAY_2) {
    if (now.hour() == START_HH && now.minute() >= START_MM){
      // keep turning on and off relays for 1 hour 
      unsigned long startTime = millis();
      while (millis() - startTime < 3600000) {
        digitalWrite(RELAY_1, HIGH); 
        delay(20000);
        digitalWrite(RELAY_1, LOW);
        delay(10000);
        digitalWrite(RELAY_2, HIGH);
        delay(20000);
        digitalWrite(RELAY_2, LOW);
        delay(10000);
      }
      digitalWrite(RELAY_1, LOW);
      digitalWrite(RELAY_2, LOW);
      delay(5000);
      } else {
      LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
      } 
  } else {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  }
}

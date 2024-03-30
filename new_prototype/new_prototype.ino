#include <SdFat.h>         
#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>  
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LowPower.h>

// Date & Time to activate Air Pumps
#define SUNDAY    0
#define MONDAY    1 
#define TUESDAY   2
#define WEDNESDAY 3
#define THURSDAY  4
#define FRIDAY    5
#define SATURDAY  6

uint8_t DAY = TUESDAY; 
uint8_t START_HH_ODD_WEEK = 12; 
uint8_t START_HH_EVEN_WEEK = 0; 
uint8_t START_MM = 0; 

char daysOfTheWeek[7][12] = {
  "Sunday",
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday"
};

// Device name
const String device = "Pondi_X";

// Data-Logger
RTC_DS1307 RTC;          
#define DS1307_I2C_ADDRESS 0x68
SdFat SD;            
const int chipSelect = 10;  
char tmeStrng[] = "0000/00/00,00:00:00"; 

// Water temperature
const int waterTempPin = 8;
OneWire oneWire(waterTempPin); 
DallasTemperature waterTempSensor(&oneWire);
float waterTempCelsius;

// Water turbidity
const int waterTurbidityPin = A3;
float minVoltage = 0;
float maxVoltage = 4.5;
int turbidityMinValue = 100;
int turbidityMaxValue = 0;

// Air pump's relay
#define RELAY_1 2
#define RELAY_2 3

void setup() {
  Serial.begin(9600);   
  
  Wire.begin();         
  RTC.begin();       

  pinMode(RELAY_1, OUTPUT); 
  pinMode(RELAY_2, OUTPUT);

  Serial.print("Blue Carbon Lab - ");
  Serial.print(device);
  Serial.println();
  Serial.println();
  Serial.print("SD card status: "); 
  if (!SD.begin(chipSelect)) {
    Serial.println("FAILED!");
    while(1);
  }
  Serial.println("PASSED!");
  Serial.println();
  Serial.print("Logging to microSD card as ");
  String filename = device + ".csv";
  Serial.println(filename);
  Serial.println();
  
  DateTime now = RTC.now();

  Serial.print("Current time: ");
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
  Serial.println();
  
  Serial.println("All setup succeed!");
  Serial.println();
  
  Serial.println("Logging data every 2 minute");
  Serial.println();

  Serial.print("Pumps are set to run on ");
  Serial.print(daysOfTheWeek[DAY]);
  Serial.print(" at:");
  Serial.println();
  Serial.print("- 1st, 3rd week: ");
  Serial.print(START_HH_EVEN_WEEK);
  Serial.print(":");
  Serial.print(START_MM);
  Serial.println();
  Serial.print("- 2nd, 4th week: ");
  Serial.print(START_HH_ODD_WEEK);
  Serial.print(":");
  Serial.print(START_MM);
  Serial.println();

  delay(500);
  
  File dataFile = SD.open(filename.c_str(), FILE_WRITE);
  if (dataFile) {
    dataFile.println("date,time,water_temp,turbidity");
    dataFile.close();
  }
  else {
    Serial.println("[!] File Error");
  }
  waterTempSensor.begin();
  for (int i = 0; i < 6; i++) {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
  }
}

void loop() {
  // Time
  DateTime now = RTC.now();  
  //Week
  int week = (now.day() + 6) / 7;   
  // Turn off pump 
  deactivate_pumps();  
  if (now.dayOfTheWeek() == DAY) {
    if (week == 1 || week == 3 || week == 5) {
      if (now.hour() == START_HH_ODD_WEEK && now.minute() >= START_MM){
        activate_pump_1();
        activate_pump_2();
      } else {
        sleep_mode();
      }
  } else if (week == 2 || week == 4) {
      if (now.hour() == START_HH_EVEN_WEEK && now.minute() >= START_MM){
        activate_pump_1();
        activate_pump_2();
      } else {
        sleep_mode();
      }
    }
  } else {
    sleep_mode();
  }
  log_data();
}

void activate_pump_1(){
  //Serial.println("[!] Pump 1 is running");
  digitalWrite(RELAY_1, LOW); 
  delay(30000);
  digitalWrite(RELAY_1, HIGH);
  //Serial.println("[!] Pump 1 has stopped");
}

void activate_pump_2(){
  //Serial.println("[!] Pump 2 is running");
  digitalWrite(RELAY_2, LOW);
  delay(30000);
  digitalWrite(RELAY_2, HIGH); 
  //Serial.println("[!] Pump 2 has stopped");
}

void deactivate_pumps(){
  digitalWrite(RELAY_1, HIGH);
  digitalWrite(RELAY_2, HIGH);
}

void log_data(){
  // Water temperature
  waterTempSensor.requestTemperatures();
  waterTempCelsius = waterTempSensor.getTempCByIndex(0);
  // Water turbidity
  int waterTurbiditySensor = analogRead(waterTurbidityPin);
  float turbidityVoltage = waterTurbiditySensor * (5.0 / 1024.0);
  //int turbidity = map(turbidityVoltage,0,4.5,100,0);
  float minVoltage = 0.0;
  float maxVoltage = 4.5;
  int newMinValue = 100;
  int newMaxValue = 0;
  int turbidity = (turbidityVoltage - minVoltage) * (newMaxValue - newMinValue) / (maxVoltage - minVoltage) + newMinValue;
  // Time
  DateTime now = RTC.now();
  sprintf(tmeStrng, "%04d/%02d/%02d,%02d:%02d:%02d", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second()); 
  String filename = device + ".csv";
  File dataFile = SD.open(filename.c_str(), FILE_WRITE);  // write the data to the SD card:
  if (dataFile) {
    dataFile.print(tmeStrng);
    dataFile.print(",");
    dataFile.print(waterTempCelsius);
    dataFile.print(",");
    dataFile.println(turbidity);
    dataFile.flush(); 
    dataFile.close();
  }
  else {
    Serial.println("[!] File Error");
  } 
}

void sleep_mode(){
  for (int i = 0; i < 6; i++) {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
  }
}

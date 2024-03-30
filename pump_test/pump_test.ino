#define RELAY1_PIN 2 
#define RELAY2_PIN 3 

void setup() {
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
}

void loop() {
  digitalWrite(RELAY1_PIN, HIGH);
  delay(30000); 
  digitalWrite(RELAY1_PIN, LOW);

  digitalWrite(RELAY2_PIN, HIGH);
  delay(30000); 
  digitalWrite(RELAY2_PIN, LOW);
}

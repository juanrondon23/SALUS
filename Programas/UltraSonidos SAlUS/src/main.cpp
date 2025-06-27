
#include <Arduino.h>

// TRIG compartido
#define TRIG_PIN  15  
// Pines ECHO de los sensores 
const int echoPins[10] = {
  13, 12, 14, 27, 26, 33, 32, 35, 34, 39
};

const uint32_t TIMEOUT_US = 100000;
const float CM_PER_US = 0.0349f;

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  digitalWrite(TRIG_PIN, LOW);

  for (int i = 0; i < 10; i++) {
    pinMode(echoPins[i], INPUT);
  }
}

void triggerAll() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
}

float readDistanceCM(int echoPin) {
  long duration = pulseIn(echoPin, HIGH, TIMEOUT_US);
  if (duration == 0) return -1.0;
  return duration * CM_PER_US / 2.0f;
}

void loop() {
  triggerAll();

  for (int i = 0; i < 10; i++) {
    float dist = readDistanceCM(echoPins[i]);
    Serial.print("Sensor ");
    Serial.print(i);
    Serial.print(": ");
    if (dist >= 0)
      Serial.print(dist);
    else
      Serial.print("Sin lectura");
    Serial.println(" cm");
    delay(50);
  }

  Serial.println("----------------------");
delay(300);
}


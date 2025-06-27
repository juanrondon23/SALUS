/*#include <Arduino.h>

#define TRIG_PIN 15
#define ECHO_PIN 13

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  digitalWrite(TRIG_PIN, LOW);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  // Disparo
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 100000);
  if (duration == 0) {
    Serial.println("Sin lectura");
  } else {
    float distance = duration * 0.0349 / 2.0;
    Serial.print("Distancia: ");
    Serial.print(distance);
    Serial.println(" cm");
  }

  delay(500);
}*/

#include <Arduino.h>
#include "led.h"

const int ledRedPin = 12;
const int ledGreenPin = 13;

bool ledRedStatus = false;
bool ledGreenStatus = false;

void configurePins() {
  pinMode(ledRedPin, OUTPUT);
  pinMode(ledGreenPin, OUTPUT);
  
  updateLEDs();
}

void updateLEDs() {
  digitalWrite(ledRedPin, ledRedStatus ? HIGH : LOW);
  digitalWrite(ledGreenPin, ledGreenStatus ? HIGH : LOW);
}

void blinkLED(int pin, int tempo, int vezes) {
  for (int i = 0; i < vezes; i++) {
    digitalWrite(pin, HIGH);
    delay(tempo);
    digitalWrite(pin, LOW);
    delay(tempo);
  }
}

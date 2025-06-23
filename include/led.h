#ifndef LED_H
#define LED_H

void configurePins();
void updateLEDs();
void blinkLED(int pin, int tempo, int vezes);

extern bool ledRedStatus;
extern bool ledGreenStatus;

#endif

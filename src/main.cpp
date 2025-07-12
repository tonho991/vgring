#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include "led.h"
#include "wifi_utils.h"
#include "file_utils.h"
#include "handlers.h"
#include "constants.h"
#include "vgring_updater.h"

ESP8266WebServer server(80);

bool previousState = HIGH;
unsigned long lastChangeTime = 0;
const unsigned long debounceDelay = 50;

bool buttonResetPressed = false;
unsigned long pressResetStartTime = 0;
const unsigned long holdResetDuration = 8000;
bool lastResetState = HIGH;

void setup() {
    Serial.begin(115200);
    configurePins();

    if(!LittleFS.begin()) {
        blinkLED(12, 100, 5);
        while(true) delay(1000);
    }

    pinMode(PIN_BTN_RING, INPUT_PULLUP);
    pinMode(PIN_BTN_RESET, INPUT_PULLUP);

    registerHandlers(server);
    server.begin();

    String ssid, pass;
    if(!(loadCredentials(ssid, pass) && wifiConnect(ssid, pass))) {
        startAccessPoint();
        blinkLED(PIN_LED_RED, 200, 4);
    } else {
        if(isAuth()){
            checkUpdate();
        }
    }
}

void loop() {
    if(IS_UPDATING) {
        server.stop();
    } else {
        server.handleClient();
    }

    // ========== BOTÃO DE RING ==========
    bool currentState = digitalRead(PIN_BTN_RING);
    unsigned long currentTime = millis();

    if(currentState != previousState && (currentTime - lastChangeTime) > debounceDelay) {
        if(IS_UPDATING) {
            blinkLED(PIN_LED_RED, 100, 3);
            return;
        }

        lastChangeTime = currentTime;
        if(currentState == LOW) {
            if(!isAuth()) {
                blinkLED(PIN_LED_RED, 100, 3);
                return;
            }
            handleRing();
        }
        previousState = currentState;
    }

    // ========== BOTÃO DE RESET ==========
    bool resetState = digitalRead(PIN_BTN_RESET);

    if(resetState == LOW && lastResetState == HIGH) {
        if(IS_UPDATING) {
            blinkLED(PIN_LED_RED, 100, 3);
            return;
        }
        pressResetStartTime = millis();
        buttonResetPressed = true;
    } else if(resetState == HIGH && lastResetState == LOW) {
        buttonResetPressed = false;
    }

    if(buttonResetPressed && (millis() - pressResetStartTime >= holdResetDuration)) {
        if(IS_UPDATING) {
            blinkLED(PIN_LED_RED, 100, 3);
            return;
        }
        buttonResetPressed = false;
        resetCredentials();
    }

    lastResetState = resetState;
}

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include "led.h"
#include "wifi_utils.h"
#include "file_utils.h"
#include "handlers.h"
#include "Globals.h"

void updateFirmware(const char* url) {
    WiFiClient client;
    IS_UPDATING = true;

    ESPhttpUpdate.setLedPin(PIN_LED_RED, LOW);

    ESPhttpUpdate.onProgress([](int current, int total) {
        blinkLED(PIN_LED_RED, 100, 1);
    });

    ESPhttpUpdate.onError([](int error) {
        blinkLED(PIN_LED_RED, 1500, 1);
        IS_UPDATING = false;
    });

    ESPhttpUpdate.onEnd([]() {
        blinkLED(PIN_LED_GREEN, 2000, 1);
        IS_UPDATING = false;
    });

    t_httpUpdate_return ret = ESPhttpUpdate.update(client, url);

    switch(ret) {
    case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

    case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;

    case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        break;
    }
}
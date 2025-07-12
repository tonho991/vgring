#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include "led.h"
#include "wifi_utils.h"
#include "file_utils.h"
#include "handlers.h"
#include "constants.h"

bool isDowngrade(const char* newVersion, const char* currentVersion) {
    String newVer = String(newVersion);
    String currVer = String(currentVersion);

    int newMajor, newMinor, newPatch;
    int currMajor, currMinor, currPatch;

    sscanf(newVer.c_str(), "v%d.%d.%d", &newMajor, &newMinor, &newPatch);
    sscanf(currVer.c_str(), "v%d.%d.%d", &currMajor, &currMinor, &currPatch);

    if (newMajor < currMajor) return true;
    if (newMajor > currMajor) return false;

    if (newMinor < currMinor) return true;
    if (newMinor > currMinor) return false;

    if (newPatch < currPatch) return true;
    return false;
}

void updateFirmware(const char* url) {
    if (strncmp(url, VGRING_FIRM_BIN_URL, strlen(VGRING_FIRM_BIN_URL)) != 0) return;

    WiFiClient client;
    IS_UPDATING = true;

    ESPhttpUpdate.setLedPin(PIN_LED_RED, LOW);

    ESPhttpUpdate.onProgress([](int current, int total) {
        blinkLED(PIN_LED_RED, 100, 1);
    });

    ESPhttpUpdate.onError([](int error) {
        IS_UPDATING = false;
        blinkLED(PIN_LED_RED, 1500, 1);
        Serial.printf("Erro na atualização: %d\n", error);
    });

    ESPhttpUpdate.onEnd([]() {
        IS_UPDATING = false;
        blinkLED(PIN_LED_GREEN, 2000, 1);
        Serial.println("Atualização concluída com sucesso.");
    });

    t_httpUpdate_return ret = ESPhttpUpdate.update(client, url);

    switch(ret) {
        case HTTP_UPDATE_FAILED:
            Serial.printf("HTTP_UPDATE_FAILED (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
            break;

        case HTTP_UPDATE_NO_UPDATES:
            Serial.println("Sem atualizações disponíveis.");
            break;

        case HTTP_UPDATE_OK:
            Serial.println("Atualização realizada com sucesso.");
            break;
    }
}

bool checkUpdate(){
    if(WiFi.status() != WL_CONNECTED) return false;

    WiFiClientSecure client;
    HTTPClient http;
    JsonDocument json;

    client.setInsecure();

    http.begin(client, VGRING_UPDATE_INFO);
    int httpCode = http.GET();

    if (httpCode <= 0) {
        Serial.printf("Erro ao obter atualização: %s\n", http.errorToString(httpCode).c_str());
        return false;
    }
    if (httpCode != 200) {
        Serial.printf("Código HTTP inesperado: %d\n", httpCode);
        return false;
    }

    DeserializationError error = deserializeJson(json, http.getString());
    if (error) {
        Serial.print(F("JSON parse error: "));
        Serial.println(error.c_str());
        return false;
    }

    if (!json.containsKey("update_url") || !json["update_url"].is<const char*>()) {
        Serial.println("'update_url' is null or invalid");
        return false;
    }

    if (!json.containsKey("version") || !json["version"].is<const char*>()) {
        Serial.println("'version' is null or invalid");
        return false;
    }

    const char* updateUrl = json["update_url"];
    const char* nVersion = json["version"];

    // OBS: -f flag is to force downgrade (Ex.: v1.0.0-f)
    if(isDowngrade(nVersion, VGRING_FIRM_VERSION) && !String(nVersion).endsWith("-f")) {
        Serial.println("Downgrade is not accepted");
        return false;
    }

    updateFirmware(updateUrl);

    return true;
}
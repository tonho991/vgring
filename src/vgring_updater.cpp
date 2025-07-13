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

int parseVersionPart(const String& s) {
  int major = 0, minor = 0, patch = 0;
  sscanf(s.c_str(), "%d.%d.%d", &major, &minor, &patch);
  return major * 10000 + minor * 100 + patch;
}


int compareVersions(String v1, String v2) {
  v1.replace("v", "");
  v2.replace("v", "");

  String v1Core = v1;
  String v2Core = v2;

  if (v1.indexOf("-") != -1) v1Core = v1.substring(0, v1.indexOf("-"));
  if (v2.indexOf("-") != -1) v2Core = v2.substring(0, v2.indexOf("-"));

  int n1 = parseVersionPart(v1Core);
  int n2 = parseVersionPart(v2Core);

  if (n1 > n2) return 1;
  if (n1 < n2) return -1;

  bool v1Pre = v1.indexOf("-") != -1;
  bool v2Pre = v2.indexOf("-") != -1;

  if (v1Pre && !v2Pre) return -1;
  if (!v1Pre && v2Pre) return 1;

  return 0;
}

bool isDowngrade(const char* newVersion, const char* currentVersion) {
  return compareVersions(String(newVersion), String(currentVersion)) < 0;
}

/*
* @brief Execute OTA process via bin URL
*
* @param url BIN URL to update
*
*/
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

/*
* @brief Check if has update available
*
* @return return true or false if has an update
*/

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

    if (!json["update_url"].is<const char*>()) {
        Serial.println("'update_url' is null or invalid");
        return false;
    }

    if (!json["version"].is<const char*>()) {
        Serial.println("'version' is null or invalid");
        return false;
    }

    const char* updateUrl = json["update_url"];
    const char* nVersion = json["version"];

    if(String(nVersion) == String(VGRING_FIRM_VERSION)){
        Serial.println("No updates availables");
        blinkLED(PIN_LED_GREEN, 200, 1);
        return false;
    }

    // OBS: -f flag is to force downgrade (Ex.: v1.0.0-f)
    if(isDowngrade(nVersion, VGRING_FIRM_VERSION) && !String(nVersion).endsWith("-f")) {
        Serial.println("Downgrade is not accepted");
        return false;
    }

        Serial.println("New version found: " + String(VGRING_FIRM_VERSION) + " > " + String(nVersion));


    updateFirmware(updateUrl);

    return true;
}
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <ESP8266HTTPClient.h>
#include "wifi_utils.h"
#include "file_utils.h"
#include "led.h"
#include "constants.h"
#include "vgring_updater.h"

extern ESP8266WebServer server;

void handleScanWifi() {
  JsonDocument json = scanWifi();

  String response;
  serializeJson(json, response);

  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, F("application/json"), response);
}

void handleConnect() {
  server.sendHeader("Access-Control-Allow-Origin", "*");

  JsonDocument doc;
  if (deserializeJson(doc, server.arg("plain"))) {
    server.send(400, F("application/json"), getJsonResponse(false, "Invalid JSON"));
    return;
  }

  String ssid = doc["ssid"];
  String pass = doc["pass"];

  if (ssid.isEmpty() || pass.isEmpty()) {
    server.send(400, F("application/json"), getJsonResponse(false, "Missing credentials"));
    return;
  }

  if (wifiConnect(ssid, pass)) {
    if (saveCredentials(ssid, pass)) {
      server.send(200, F("application/json"), "{\"status\":\"Conect successfull!\", \"ip\":\"" + WiFi.localIP().toString() + "\"}");
      delay(2000);
      WiFi.softAPdisconnect(true);
      WiFi.mode(WIFI_STA);
    } else {
      server.send(500, F("application/json"), getJsonResponse(false, "Failed to save credentials"));
    }
  } else {
    server.send(500, F("application/json"), getJsonResponse(false, "Connection failed"));
  }
}

void handleGetConfig() {
  server.sendHeader("Access-Control-Allow-Origin", "*");

  JsonDocument wifiJson, configJson, response;

  if (!deserializeJson(wifiJson, readFile("/db/wifi.json")) ||
      !deserializeJson(configJson, readFile("/db/config.json"))) {
    server.send(500, F("application/json"), getJsonResponse(false, "Failed to read config"));
    return;
  }

  response["token"] = configJson["token"] | "";
  response["wifi"] = wifiJson["ssid"] | "";
  response["version"] = VGRING_FIRM_VERSION;

  String res;
  serializeJson(response, res);

  server.send(200, F("application/json"), res);
}

void handleSign() {
  server.sendHeader("Access-Control-Allow-Origin", "*");

  JsonDocument doc;
  if (deserializeJson(doc, server.arg("plain"))) {
    server.send(400, F("application/json"), getJsonResponse(false, "Invalid JSON body"));
    return;
  }

  String token = doc["token"];
  if (token.isEmpty()) {
    server.send(400, F("application/json"), getJsonResponse(false, "Missing token"));
    return;
  }

  File file = LittleFS.open("/db/config.json", "w");
  if (!file) {
    server.send(500, F("application/json"), getJsonResponse(false, "Failed to write file"));
    return;
  }

  serializeJson(doc, file);
  file.close();
  server.send(200, F("application/json"), getJsonResponse(true, "Token saved"));
}

void handleNotFound() {
  String path = server.uri();
  if (path == "/") path = "/index.html";

  if (path.startsWith("/db")) {
    server.send(403, F("text/plain"), "403: Forbidden");
    return;
  }

  if (!LittleFS.exists(path)) {
    server.send(404, F("text/plain"), "404: File Not Found");
    return;
  }

  File file = LittleFS.open(path, "r");
  server.streamFile(file, F("text/html"));
  file.close();
}

void registerHandlers(ESP8266WebServer& s) {
  s.on("/api/scan_wifi", HTTP_GET, handleScanWifi);
  s.on("/api/connect", HTTP_POST, handleConnect);
  s.on("/api/get_config", HTTP_GET, handleGetConfig);
  s.on("/api/sign", HTTP_POST, handleSign);
  s.onNotFound(handleNotFound);
}

void handleRing() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClientSecure client;
    client.setInsecure();

    http.begin(client, "https://vgring.vgapp.com.br/api/ring");
    http.addHeader("Content-Type", "application/json");

    String jsonPayload = "{\"token\":\"" + getUserToken() + "\"}";

    int httpCode = http.POST(jsonPayload);

    if (httpCode > 0) {
      String payload = http.getString();
      blinkLED(PIN_LED_GREEN, 300, 1);
    } else {
      blinkLED(PIN_LED_RED, 300, 1);
    }
    http.end();
  } else {
    blinkLED(PIN_LED_RED, 300, 3);
  }
}
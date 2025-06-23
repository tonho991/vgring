#include <LittleFS.h>
#include <ArduinoJson.h>
#include "file_utils.h"
#include "led.h"

String readFile(const String& path) {
  if (!LittleFS.exists(path)) return "";

  File file = LittleFS.open(path, "r");
  if (!file || file.isDirectory()) return "";

  String content = file.readString();
  file.close();
  return content;
}

bool writeFile(const String& path, const String& content) {
  File file = LittleFS.open(path, "w");
  if (!file) {
    blinkLED(12, 200, 4);
    return false;
  }

  file.print(content);
  file.close();
  return true;
}

bool saveCredentials(const String& ssid, const String& pass) {
  JsonDocument doc;
  doc["ssid"] = ssid;
  doc["pass"] = pass;

  String json;
  serializeJson(doc, json);
  return writeFile("/db/wifi.json", json);
}

bool loadCredentials(String& ssid, String& pass) {
  String file = readFile("/db/wifi.json");
  if (file == "") return false;

  JsonDocument doc;
  if (deserializeJson(doc, file)) return false;

  ssid = doc["ssid"].as<String>();
  pass = doc["pass"].as<String>();

  return ssid != "default_vgwifi" && ssid != "";
}

bool isAuth() {
  String file = readFile("/db/config.json");
  if (file == "") return false;

  JsonDocument doc;
  if (deserializeJson(doc, file)) return false;

  String token = doc["token"].as<String>();
  return !(token == "default_token" || token == "");
}

String getUserToken() {
  String file = readFile("/db/config.json");
  if (file == "") return String();

  JsonDocument doc;
  if (deserializeJson(doc, file)) return String();

  return doc["token"].as<String>();
}

const char* getJsonResponse(const bool success, const char* msg) {
  static char buffer[256];
  snprintf(buffer, sizeof(buffer),
           "{\"status\":\"%s\",\"msg\":\"%s\"}",
           success ? "success" : "error",
           msg);
  return buffer;
}

void resetCredentials() {
  JsonDocument wifi;
  JsonDocument config;

  wifi["ssid"] = "default_vgwifi";
  wifi["pass"] = "default_password";
  config["token"] = "default_token";

  String wifiJsonStr, configJsonStr;
  serializeJson(wifi, wifiJsonStr);
  serializeJson(config, configJsonStr);

  writeFile("/db/wifi.json", wifiJsonStr);
  writeFile("/db/config.json", configJsonStr);

  blinkLED(12, 500, 1);
  delay(1000);
  ESP.restart();
}

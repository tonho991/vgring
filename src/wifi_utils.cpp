#include <ESP8266WiFi.h>
#include "wifi_utils.h"
#include "led.h"

const IPAddress apIP(192, 168, 4, 1);
const IPAddress apGateway(192, 168, 4, 1);
const IPAddress apSubnet(255, 255, 255, 0);

bool wifiConnect(const String& ssid, const String& password) {
  WiFi.begin(ssid, password);

  for (int i = 0; i < 20; i++) {
    if (WiFi.status() == WL_CONNECTED) break;
    blinkLED(12, 100, 1);
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED) {
    blinkLED(13, 300, 2);
    return true;
  }

  blinkLED(12, 1500, 1);
  return false;
}

String getEncryptionType(int type) {
  switch (type) {
    case ENC_TYPE_WEP: return "WEP";
    case ENC_TYPE_TKIP: return "WPA";
    case ENC_TYPE_CCMP: return "WPA2";
    case ENC_TYPE_NONE: return "Open";
    case ENC_TYPE_AUTO: return "Auto";
    default: return "Unknown";
  }
}

JsonDocument scanWifi() {
  int n = WiFi.scanNetworks();
  JsonDocument doc;
  JsonArray arr = doc.to<JsonArray>();

  if (n <= 0) return doc;

  int32_t indices[n];
  for (int i = 0; i < n; i++) indices[i] = i;

  std::sort(indices, indices + n, [](int a, int b) {
    return WiFi.RSSI(a) > WiFi.RSSI(b);
  });

  for (int i = 0; i < n; i++) {
    int idx = indices[i];
    String ssid = WiFi.SSID(idx);
    if (ssid.length() == 0) continue;

    JsonObject net = arr.add<JsonObject>();
    net["ssid"] = ssid;
    net["rssi"] = WiFi.RSSI(idx);
    net["encryption"] = getEncryptionType(WiFi.encryptionType(idx));
  }

  return doc;
}

void startAcessPoint(){
  Serial.println("Modo AP: configuração de Wi-Fi");
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apGateway, apSubnet);
  WiFi.softAP("VGRing Connect");

  Serial.println("AP criado com IP: " + WiFi.softAPIP().toString());
}


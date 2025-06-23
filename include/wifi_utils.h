#ifndef WIFI_UTILS_H
#define WIFI_UTILS_H

#include <ArduinoJson.h>

bool wifiConnect(const String& ssid, const String& password);
JsonDocument scanWifi();
String getEncryptionType(int type);
void startAcessPoint();
void handleRing();

#endif

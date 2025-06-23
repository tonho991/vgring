#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <Arduino.h>

String readFile(const String& path);
bool writeFile(const String& path, const String& content);
bool saveCredentials(const String& ssid, const String& pass);
bool loadCredentials(String& ssid, String& pass);
bool isAuth();
String getUserToken();
void resetCredentials();
const char* getJsonResponse(const bool success, const char* msg);

#endif

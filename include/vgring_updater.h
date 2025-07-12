#ifndef FIRM_UPDATER_H
#define FIRM_UPDATER_H

void updateFirmware(const char* url);
bool isDowngrade(const char* newVersion, const char* currentVersion);
bool checkUpdate();

#endif
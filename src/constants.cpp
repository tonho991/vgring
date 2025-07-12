#include "constants.h"

// ====== PINS =======

int PIN_LED_RED = 12;    // D6
int PIN_LED_GREEN = 13;  // D7

int PIN_BTN_RING = 14;    // D5
int PIN_BTN_RESET = 5;    // D1

// ====== Constants =====

char* VGRING_FIRM_VERSION = "v1.0.1";
char* VGRING_API_URL = "https://vgring.vgapp.com.br/api";
char* VGRING_FIRM_BIN_URL = "https://github.com/tonho991/vgring/releases/download/";
char* VGRING_UPDATE_INFO = "https://raw.githubusercontent.com/tonho991/vgring/refs/heads/master/update_info.json";

bool IS_UPDATING = false;
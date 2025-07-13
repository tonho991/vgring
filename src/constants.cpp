#include "constants.h"

// ====== Pins ======
const int PIN_LED_RED    = 12;
const int PIN_LED_GREEN  = 13;
const int PIN_BTN_RING   = 14;
const int PIN_BTN_RESET  = 5;

// ====== Firmware ======
const char* const VGRING_FIRM_VERSION = "v1.0.1";
const char* const VGRING_API_URL      = "https://vgring.vgapp.com.br/api";
const char* const VGRING_FIRM_BIN_URL = "https://github.com/tonho991/vgring/releases/download/";
const char* const VGRING_UPDATE_INFO  = "https://raw.githubusercontent.com/tonho991/vgring/refs/heads/master/update_info.json";

// ====== States ======
bool IS_UPDATING = false;
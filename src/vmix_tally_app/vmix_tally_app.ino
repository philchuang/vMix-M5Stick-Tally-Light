#define ESP32

#include <HardwareSerial.h>
#include <M5StickC.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <EEPROM.h>
#include <PinButton.h>

#include "AppSettings.h"
#include "AppSettingsManager.h"
#include "BatteryLevel.h"

#define LED_BUILTIN 10
#define EEPROM_SIZE 512
#define CLEAR_SETTINGS_ON_LOAD false

#define SCREEN_START 0
#define SCREEN_SETTINGS 1
#define SCREEN_TALLY 2
#define SCREEN_TALLY_NR 3
#define SCREEN_ERROR 255

#define TALLY_NONE '?'
#define TALLY_SAFE '0'
#define TALLY_LIVE '1'
#define TALLY_PRE  '2'

#define TALLY_NR_MAX 30

#define VMIX_CONN_RETRIES 3
#define VMIX_KEEPALIVE_MS 5000
#define VMIX_RESPONSE_MS 100
#define APP_ORIENTATION_MS 500
#define APP_SCREENREFRESH_MS 5000
#define M5_CHARGING_MS 5000
#define M5_BATTERYLEVEL_MS 30000

#define FONT 1

// GLOBAL STATE
AppSettings settings;
// WebServer server(80);
char currentTallyState = TALLY_NONE;
byte currentScreen = SCREEN_START;
unsigned int conn_Reconnections = 0;
bool isCharging = false;
double currentBatteryLevel;
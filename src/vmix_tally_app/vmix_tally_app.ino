#define ESP32

#include <HardwareSerial.h>
#include <M5StickC.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <PinButton.h>

#include "AppSettings.h"
#include "AppSettingsManager.h"
#include "BatteryManager.h"
#include "OrientationManager.h"

#define LED_BUILTIN 10

#define CLEAR_SETTINGS_ON_LOAD false
#define PREPARE_BATTERY_LOGGING false
#define HIGH_VIZ_MODE false

#define SCREEN_START    0
#define SCREEN_SETTINGS 1
#define SCREEN_TALLY    2
#define SCREEN_TALLY_NR 3
#define SCREEN_ERROR    255

#define TALLY_NONE '?'
#define TALLY_SAFE '0'
#define TALLY_LIVE '1'
#define TALLY_PRE  '2'

#define TALLY_NR_MAX 30

#define VMIX_CONN_RETRIES 3
#define VMIX_KEEPALIVE_MS 5000
#define VMIX_RESPONSE_MS 100
#define APP_ORIENTATION_MS 500
#define APP_SCREENREFRESH_MS 10000
#define M5_CHARGING_MS 5000
#define M5_BATTERYLEVEL_MS 30000
#define APP_BRIGHTNESS_TIMEOUT_MS 1000
#define APP_ROTATION_THRESHOLD 0.8f
#define FONT 1

// GLOBAL STATE
AppSettings settings;
// WebServer server(80);
char currentTallyState;
byte currentScreen;
unsigned int conn_Reconnections;
bool isCharging;
double currentBatteryLevel;
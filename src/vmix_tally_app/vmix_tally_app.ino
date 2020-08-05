// this file only contains includes

// hardware
#define ESP32
#include <HardwareSerial.h>
#include <M5StickC.h>
#include <WiFi.h>
#include <SPIFFS.h>

// libraries
#include <string>
#include <vector>
#include <PinButton.h>
#include <Callback.h>

// app
#include "AppContext.h"
#include "AppSettings.h"
#include "AppSettingsManager.h"
#include "BatteryManager.h"
#include "LoopEvent.h"
#include "OrientationManager.h"
#include "Screen.h"
#include "ScreenManager.h"
#include "VmixManager.h"
#include "WifiManager.h"

// #define CLEAR_SETTINGS_ON_LOAD false
// #define LOG_BATTERY false
// #define PREPARE_BATTERY_LOGGING false
// #define HIGH_VIZ_MODE false

// #define SCREEN_START    1
// #define SCREEN_CONN     2
// #define SCREEN_TALLY    3
// #define SCREEN_TALLY_NR 6 // temporary
// #define SCREEN_SETTINGS 5
// #define SCREEN_ERROR    0
// #define SCREEN_COUNT    7 // highest number + 1

// #define TALLY_NONE '?'
// #define TALLY_SAFE '0'
// #define TALLY_LIVE '1'
// #define TALLY_PRE  '2'

// #define TALLY_NR_MAX 30

// #define VMIX_CONN_RETRIES 3
// #define VMIX_KEEPALIVE_MS 5000u
// #define VMIX_RESPONSE_MS 100u
// #define APP_ORIENTATION_MS 500u
// #define APP_SCREENREFRESH_MS 10000u
// #define M5_CHARGING_MS 5000u
// #define M5_BATTERYLEVEL_MS 30000u
// #define APP_BRIGHTNESS_TIMEOUT_MS 1000u
// #define APP_ROTATION_THRESHOLD 0.8f
// #define FONT 1

// // GLOBAL STATE
// AppSettings settings;
// // WebServer server(80);
// char currentTallyState;
// byte currentScreen;
// unsigned int conn_Reconnections;
// bool isCharging;
// double currentBatteryLevel;
// AppContext *_context;

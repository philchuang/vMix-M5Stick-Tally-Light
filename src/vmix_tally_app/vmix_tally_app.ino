/* 
 * this file only contains includes
 */

// hardware
#define ESP32
#include <Arduino.h>
#include <HardwareSerial.h>
#include <M5StickC.h>
#include <WiFi.h>
#include <SPIFFS.h>

// libraries
#include <string>
#include <vector>
#include <PinButton.h>
#include <Callback.h>

// app headers
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

// app implementations
#include "ConnectingScreen.cpp"
#include "ErrorScreen.cpp"
#include "SettingsScreen.cpp"
#include "SplashScreen.cpp"
#include "TallyScreen.cpp"

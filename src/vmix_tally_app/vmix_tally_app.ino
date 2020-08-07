/* 
 * includes
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

/* 
 * default network configuration
 */

// HOME
#define SETTINGS0_WIFI_SSID "***REMOVED***"
#define SETTINGS0_WIFI_PASS "***REMOVED***"
#define SETTINGS0_VMIX_ADDR "***REMOVED***" // or hostname
#define SETTINGS0_VMIX_PORT 8099 // default TCP API port
#define SETTINGS0_TALLY_NR 1 // initial tally number

***REMOVED***
#define SETTINGS1_WIFI_SSID "***REMOVED***"
#define SETTINGS1_WIFI_PASS "***REMOVED***"
#define SETTINGS1_VMIX_ADDR "***REMOVED***" // or hostname
#define SETTINGS1_VMIX_PORT 8099 // default TCP API port
#define SETTINGS1_TALLY_NR 1 // initial tally number

/* 
 * global constants
 */

#define LED_BUILTIN 10

/* 
 * global constants that define behavior
 */

#define HIGH_VIZ_MODE 1
#define CLEAR_SETTINGS_ON_LOAD false

/* 
 * the core of the application: setup() and loop()
 */

// globals
AppContext *_context;
ScreenManager *_screenMgr;

void setup()
{
    // hardware initialization
    initHardware();

    // context and services initialization
    auto context = AppContext();
    _context = &context;
    _context->begin();

    initPower();

    initSettings();

    initScreens();
}

void loop()
{
}

void initHardware()
{
    M5.begin();
    Serial.begin(115200);
    SPIFFS.begin();
    pinMode(LED_BUILTIN, OUTPUT);
}

void initPower()
{
    // power savings
    setCpuFrequencyMhz(80);
    btStop();

    if (HIGH_VIZ_MODE)
    {
        _context->setBacklight(100);
    }
    else
    {
        _context->setBacklight(60);
    }
    _context->getBatteryManager()->setLedOn(false);
}

void initSettings()
{
    if (CLEAR_SETTINGS_ON_LOAD)
    {
        Serial.println("Clearing EEPROM...");

        auto settingsMgr = _context->getSettingsManager();

        for (auto i = 0; i < settingsMgr->getNumSettings(); i++)
        {
            settingsMgr->clear(i);
        }
        settingsMgr->saveUptimeInfo(0, 0);

        Serial.println("EEPROM cleared.");
    }
}

void initScreens()
{
    auto screenMgr = ScreenManager(*_context, MAX_SCREENS);
    _screenMgr = &screenMgr;
    SplashScreen splash(*_context);
    Screen *screen = &splash;
    _screenMgr->add(screen);
}

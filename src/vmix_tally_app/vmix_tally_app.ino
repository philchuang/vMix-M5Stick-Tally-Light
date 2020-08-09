
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
#include "Constants.h"
#include "Configuration.h"
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
 * the core of the application: setup() and loop()
 */

// globals
AppContext *_context;
ScreenManager *_screenMgr;
bool _saveUptimeInfo = false; // TEMPORARY

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

    if (LOG_BATTERY)
    {
        if (PREPARE_BATTERY_LOGGING)
        {
            _context->getSettingsManager()->saveUptimeInfo(0, 0);
            _saveUptimeInfo = false;
        }
        else
        {
            delay(10000);
        }
        if (_context->getSettingsManager()->getLastUptime() != 0)
        {
            _saveUptimeInfo = false;
        }
    }

    _screenMgr->show(SCREEN_SPLASH);
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

    Screen *screen;

    ErrorScreen errScr(*_context);
    screen = &errScr;
    _screenMgr->add(screen);

    SplashScreen splash(*_context);
    screen = &splash;
    _screenMgr->add(screen);

    ConnectingScreen connect(*_context);
    screen = &connect;
    _screenMgr->add(screen);

    TallyScreen tally(*_context, HIGH_VIZ_MODE);
    screen = &tally;
    _screenMgr->add(screen);

    SettingsScreen settings(*_context);
    screen = &settings;
    _screenMgr->add(screen);
}

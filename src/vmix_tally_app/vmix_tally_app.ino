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
#include "OrientationManager.h"
#include "Screen.h"
#include "ScreenManager.h"
#include "VmixManager.h"
#include "WifiManager.h"

/* 
 * the core of the application: setup() and loop()
 */

// globals
AppContext _context;
ScreenManager _screenMgr(_context, MAX_SCREENS);
PinButton btnM5(BTN_M5);
PinButton btnSide(BTN_SIDE);

void setup()
{
    // hardware initialization
    initHardware();

    // context and services initialization
    _context.begin();

    initPower();

    initSettings();

    initScreens();

    _screenMgr.show(SCREEN_SPLASH);
}

void loop()
{
    unsigned long timestamp = millis();

    _context.handleLoop(timestamp);

    btnM5.update();
    btnSide.update();
    _screenMgr.handleInput(timestamp, btnM5, btnSide);
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
        _context.setBacklight(100);
    }
    else
    {
        _context.setBacklight(60);
    }
    _context.getBatteryManager()->setLedOn(false);
}

void initSettings()
{
    if (CLEAR_SETTINGS_ON_LOAD)
    {
        Serial.println("Clearing EEPROM...");

        auto settingsMgr = _context.getSettingsManager();

        for (auto i = 0; i < settingsMgr->getNumSettings(); i++)
        {
            settingsMgr->clear(i);
        }

        Serial.println("EEPROM cleared.");
    }
}

void initScreens()
{
    _screenMgr.begin();
}

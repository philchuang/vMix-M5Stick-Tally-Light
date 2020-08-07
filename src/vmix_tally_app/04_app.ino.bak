/* 
 * the core of the application: setup() and loop()
 */

// ----- intellisense support only, comment out before building -----
// // hardware
// #define ESP32
// #include <Arduino.h>
// #include <M5StickC.h>

// // app headers
// #include "AppContext.h"
// #include "AppSettingsManager.h"
// #include "ScreenManager.h"

// // app impls
// #include "ConnectingScreen.cpp"
// #include "ErrorScreen.cpp"
// #include "SettingsScreen.cpp"
// #include "SplashScreen.cpp"
// #include "TallyScreen.cpp"

// // constants
// #define LED_BUILTIN 10
// #define HIGH_VIZ_MODE 1
// #define CLEAR_SETTINGS_ON_LOAD false
// ----- intellisense support only, comment out before building -----

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

/* 
 * the core of the application: setup() and loop()
 */

// ----- intellisense support only, comment out before building -----
// // hardware
// #define ESP32
// #include <M5StickC.h>

// // libraries
// #include "AppContext.h"
// #include "AppSettingsManager.h"

// // constants
// #define LED_BUILTIN 10
// #define HIGH_VIZ_MODE 1
// #define CLEAR_SETTINGS_ON_LOAD false
// ----- intellisense support only, comment out before building -----

// globals
AppContext *_context;

void setup()
{
    // power savings
    setCpuFrequencyMhz(80);
    btStop();

    // hardware initialization
    M5.begin();
    Serial.begin(115200);
    SPIFFS.begin();
    pinMode(LED_BUILTIN, OUTPUT);

    // context and services initialization
    _context = &AppContext();
    _context->begin();

    // power setup
    if (HIGH_VIZ_MODE)
    {
        _context->setBrightness(100);
    }
    else
    {
        _context->setBrightness(60);
    }
    _context->getBatteryManager()->setLedOn(false);

    if (CLEAR_SETTINGS_ON_LOAD)
    {
        Serial.println("Clearing EEPROM...");

        auto settingsMgr = _context->getSettingsManager();

        for (auto it = 0; it < settingsMgr->getNumSettings(); ++it)
        {
            settingsMgr.clear(settingsIdx);
        }
        settingsMgr.saveUptimeInfo(0, 0);

        Serial.println("EEPROM cleared.");
    }

    _context->scree
}

void loop()
{
}
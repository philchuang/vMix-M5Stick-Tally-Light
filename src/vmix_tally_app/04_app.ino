// intellisense support only, comment out before building
// hardware
#define ESP32
#include <M5StickC.h>

// libraries
#include "AppContext.h"

// constants
#define HIGH_VIZ_MODE 1
#define CLEAR_SETTINGS_ON_LOAD false

// globals
AppContext *_context;
// intellisense support only, comment out before building

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
        // TODO implement
        // settings_clear();
    }
}

void loop()
{
}
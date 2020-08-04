#define ESP32

#define HIGH_VIZ_MODE 1

#include <M5StickC.h>
#include "AppContext.h"

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

    // context and services initialization
    _context = &AppContext();
    _context->begin();
    if (HIGH_VIZ_MODE)
    {
        _context->setBacklight(100);
    }
    else
    {
        _context->setBacklight(60);
    }

    
}

void loop()
{
}
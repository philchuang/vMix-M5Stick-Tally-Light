#define ESP32
#define MIN_BRIGHTNESS 8
#define MAX_BRIGHTNESS 12

// #include <Wire.h>
#include <M5StickC.h>
#include <AXP192.h>
#include "BatteryManager.h"

struct BatteryManager::Impl
{
    Impl() {}
    ~Impl() {}

    unsigned int _backlight = MAX_BRIGHTNESS;
};

BatteryManager::BatteryManager() : _pimpl(new Impl()) {}

BatteryManager::~BatteryManager() = default;

void BatteryManager::begin()
{
    M5.Axp.begin();
}

unsigned int BatteryManager::cycleBacklight()
{
    _pimpl->_backlight = _pimpl->_backlight + 1;
    if (_pimpl->_backlight > MAX_BRIGHTNESS)
        _pimpl->_backlight = MIN_BRIGHTNESS;
    M5.Axp.ScreenBreath(_pimpl->_backlight);
    return 100 * (_pimpl->_backlight - MIN_BRIGHTNESS + 1) / (MAX_BRIGHTNESS - MIN_BRIGHTNESS + 1);
}

void BatteryManager::setBacklight(unsigned int brightness)
{
    unsigned int backlight = brightness / (100 / (MAX_BRIGHTNESS - MIN_BRIGHTNESS + 1)) - 1 + MIN_BRIGHTNESS;
    if (backlight < MIN_BRIGHTNESS) backlight = MIN_BRIGHTNESS;
    else if (backlight > MAX_BRIGHTNESS) backlight = MAX_BRIGHTNESS;
    _pimpl->_backlight = backlight;
    M5.Axp.ScreenBreath(_pimpl->_backlight);
}
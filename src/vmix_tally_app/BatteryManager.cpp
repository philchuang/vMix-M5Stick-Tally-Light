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

BatteryManager::BatteryManager()
    : _pimpl(new Impl())
{
    M5.Axp.begin();
}

BatteryManager::~BatteryManager() = default;

unsigned int BatteryManager::cycleBacklight()
{
    _pimpl->_backlight = _pimpl->_backlight + 1;
    if (_pimpl->_backlight > MAX_BRIGHTNESS)
        _pimpl->_backlight = MIN_BRIGHTNESS;
    M5.Axp.ScreenBreath(_pimpl->_backlight);
    return 100 * (_pimpl->_backlight - MIN_BRIGHTNESS + 1) / (MAX_BRIGHTNESS - MIN_BRIGHTNESS + 1);
}
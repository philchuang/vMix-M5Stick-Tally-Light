#include "BatteryManager.h"

// hardware
#define ESP32
#include <M5StickC.h>
#include <AXP192.h>

// constants
#define UNKNOWN_CONSTANT 1.1    // seriously what is this
#define MAX_WORKING_VOLTAGE 4.1 // 4.07
#define MIN_WORKING_VOLTAGE 3.07
#define LED_BUILTIN 10
#define MIN_BRIGHTNESS 8
#define MAX_BRIGHTNESS 12

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

double BatteryManager::getBatteryLevel()
{
    // https://forum.m5stack.com/topic/1361/ischarging-and-getBatteryManager/7

    // impl #1
    /*
    unsigned int vbatData = M5.Axp.GetVbatData(); // docs: "Get the battery voltage value"
    double vbat = vbatData * 1.1 / 1000;
    // charging: 4.188 V
    // unplugged: 4.068 V (fully charged)
    // unplugged: 3.068 V (45 minutes later, about to turn off)

    unsigned int vapsData = M5.Axp.GetVapsData(); // docs: "Get battery capacity"
    double vaps = vapsData * 1.4 / 1000;
    // charging: 4.971 V
    // unplugged: 4.068 V (fully charged)
    // unplugged: 3.066 V (45 minutes later, about to turn off)

    double BatteryManager = 100.0 * ((vaps - 3.0) / (vbat - 3.0));
    M5.Lcd.printf("BatteryManager: %.0f%%    ", BatteryManager);
    */

    // TODO confirm minimum working voltage

    // impl #2
    unsigned int vbatData = M5.Axp.GetVbatData();
    double vbat = vbatData * UNKNOWN_CONSTANT / 1000;
    return min(100.0 * ((vbat - MIN_WORKING_VOLTAGE) / (MAX_WORKING_VOLTAGE - MIN_WORKING_VOLTAGE)), 100.0);
}

bool BatteryManager::isCharging()
{
    return M5.Axp.GetInputPowerStatus() >> 2 > 0;
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
    if (backlight < MIN_BRIGHTNESS)
        backlight = MIN_BRIGHTNESS;
    else if (backlight > MAX_BRIGHTNESS)
        backlight = MAX_BRIGHTNESS;
    _pimpl->_backlight = backlight;
    M5.Axp.ScreenBreath(_pimpl->_backlight);
}

void BatteryManager::setLedOn(bool isOn)
{
    if (isOn)
        digitalWrite(LED_BUILTIN, LOW);
    else
        digitalWrite(LED_BUILTIN, HIGH);
}

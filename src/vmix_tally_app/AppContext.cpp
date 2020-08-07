#include "AppContext.h"

// hardware
#define ESP32
#include <M5StickC.h>

// libraries
#include <stdio.h>
#include <string>

// app
#include "AppSettings.h"
#include "AppSettingsManager.h"
#include "BatteryManager.h"
#include "OrientationManager.h"
#include "Screen.h"
// #include "ScreenManager.h"
#include "WifiManager.h"
#include "VmixManager.h"

// constants
#define EEPROM_SIZE 512
#define APP_ROTATION_THRESHOLD 0.8f
// APPSETTINGS_SIZE * MAX_SETTINGS_NR cannot exceed EEPROM_SIZE
#ifdef SETTINGS1_WIFI_SSID
#define MAX_SETTINGS_NR 2
#else
#define MAX_SETTINGS_NR 1
#endif

struct AppContext::Impl
{
    Impl()
    {
    }

    ~Impl()
    {
    }

    AppSettingsManager *_appSettingsMgr;
    unsigned short _appSettingsIdx;
    AppSettings *_appSettings;

    WifiManager *_wifiMgr;
    bool _isWifiConnected;
    unsigned int _numReconnections;

    VmixManager *_vmixMgr;
    bool _isVmixConnected;
    char _tallyState;

    OrientationManager *_orientationMgr;

    BatteryManager *_batteryMgr;
    bool _isCharging;
    double _batteryLevel;

    // ScreenManager *_screenMgr;
};

AppContext::AppContext()
    : _pimpl(new Impl())
{
}

AppContext::~AppContext()
{
    _pimpl->_appSettingsMgr->~AppSettingsManager();
    delete _pimpl->_appSettingsMgr;
    delete _pimpl->_appSettings;
    _pimpl->_wifiMgr->~WifiManager();
    delete _pimpl->_wifiMgr;
    _pimpl->_vmixMgr->~VmixManager();
    delete _pimpl->_vmixMgr;
    _pimpl->_orientationMgr->~OrientationManager();
    delete _pimpl->_orientationMgr;
    _pimpl->_batteryMgr->~BatteryManager();
    delete _pimpl->_batteryMgr;
};

void AppContext::begin()
{
    auto settingsMgr = AppSettingsManager(EEPROM_SIZE, MAX_SETTINGS_NR);
    settingsMgr.begin();
    _pimpl->_appSettingsMgr = &settingsMgr;

    auto wifi = WifiManager();
    _pimpl->_wifiMgr = &wifi;

    auto vmix = VmixManager();
    _pimpl->_vmixMgr = &vmix;

    auto orientationMgr = OrientationManager(APP_ROTATION_THRESHOLD);
    orientationMgr.begin();
    _pimpl->_orientationMgr = &orientationMgr;

    auto batt = BatteryManager();
    batt.begin();
    _pimpl->_batteryMgr = &batt;

    // auto screen = ScreenManager(&this, MAX_SCREENS);
    // screen.begin();
    // // TODO add screens
    // _pimpl->_screenMgr = &screen;
}

AppSettingsManager *AppContext::getSettingsManager()
{
    return _pimpl->_appSettingsMgr;
}

unsigned short AppContext::getSettingsIdx()
{
    return _pimpl->_appSettingsIdx;
}

unsigned short AppContext::getNumSettings()
{
    return _pimpl->_appSettingsMgr->getNumSettings();
}

AppSettings *AppContext::getSettings()
{
    return _pimpl->_appSettings;
}

AppSettings *AppContext::loadSettings(unsigned short settingsIdx)
{
    auto settings = _pimpl->_appSettingsMgr->load(settingsIdx);
    _pimpl->_appSettings = &settings;
    _pimpl->_appSettingsIdx = settingsIdx;
    return _pimpl->_appSettings;
}

AppSettings *AppContext::cycleSettings()
{
    auto idx = (_pimpl->_appSettingsIdx + 1) % MAX_SETTINGS_NR;
    return this->loadSettings(idx);
}

WifiManager *AppContext::getWifiManager()
{
    return _pimpl->_wifiMgr;
}

// TODO replace with call to isAlive()?
bool AppContext::getIsWifiConnected()
{
    return _pimpl->_isWifiConnected;
}

void AppContext::setIsWifiConnected(bool connected)
{
    _pimpl->_isWifiConnected = connected;
}

unsigned int AppContext::getNumReconnections()
{
    return _pimpl->_numReconnections;
}

void AppContext::incNumReconnections()
{
    _pimpl->_numReconnections++;
}

VmixManager *AppContext::getVmixManager()
{
    return _pimpl->_vmixMgr;
}

// TODO replace with call to isAlive()?
bool AppContext::getIsVmixConnected()
{
    return _pimpl->_isVmixConnected;
}

void AppContext::setIsVmixConnected(bool connected)
{
    _pimpl->_isVmixConnected = connected;
}

char AppContext::getTallyState()
{
    return _pimpl->_tallyState;
}

void AppContext::setTallyState(char state)
{
    _pimpl->_tallyState = state;
}

OrientationManager *AppContext::getOrientationManager()
{
    return _pimpl->_orientationMgr;
}

bool AppContext::getOrientation()
{
    return _pimpl->_orientationMgr->getOrientation();
}

BatteryManager *AppContext::getBatteryManager()
{
    return _pimpl->_batteryMgr;
}

// TODO replace with call to isCharging()?
bool AppContext::getIsCharging()
{
    return _pimpl->_isCharging;
}

void AppContext::setIsCharging(bool charging)
{
    _pimpl->_isCharging = charging;
}

// TODO replace with call to getBatteryLevel()?
double AppContext::getBatteryLevel()
{
    return _pimpl->_batteryLevel;
}

void AppContext::setBatteryLevel(double battery)
{
    _pimpl->_batteryLevel = battery;
}

unsigned int AppContext::cycleBacklight()
{
    return _pimpl->_batteryMgr->cycleBacklight();
}

void AppContext::setBacklight(unsigned int brightness)
{
    _pimpl->_batteryMgr->setBacklight(brightness);
}

// ScreenManager *AppContext::getScreenManager()
// {
//     return _pimpl->_screenMgr;
// }
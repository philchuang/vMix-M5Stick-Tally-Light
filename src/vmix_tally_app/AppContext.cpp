#define ESP32

#define EEPROM_SIZE 512
#define APP_ROTATION_THRESHOLD 0.8f

// APPSETTINGS_SIZE * MAX_SETTINGS_NR cannot exceed EEPROM_SIZE
#ifdef SETTINGS1_WIFI_SSID
#define MAX_SETTINGS_NR 2
#else
#define MAX_SETTINGS_NR 1
#endif

#include "AppContext.h"
#include <stdio.h>
#include <string>
#include <M5StickC.h>
#include "AppSettings.h"
#include "AppSettingsManager.h"
#include "WifiManager.h"
#include "VmixManager.h"
#include "OrientationManager.h"
#include "BatteryManager.h"

struct AppContext::Impl
{
    Impl()
    {
    }

    ~Impl()
    {
    }

    AppSettings *_appSettings;
    unsigned short _appSettingsIdx;
    AppSettingsManager *_appSettingsMgr;
    bool _isWifiConnected;
    bool _isVmixConnected;
    char _tallyState;
    unsigned int _numReconnections;
    bool _isCharging;
    double _batteryLevel;
    OrientationManager *_orientationMgr;
    BatteryManager *_batteryMgr;
    WifiManager *_wifiMgr;
    VmixManager *_vmixMgr;
};

AppContext::AppContext()
    : _pimpl(new Impl())
{
}

AppContext::~AppContext()
{
    delete _pimpl->_appSettings;
    delete _pimpl->_appSettingsMgr;
    delete _pimpl->_wifiMgr;
    delete _pimpl->_vmixMgr;
    delete _pimpl->_orientationMgr;
    delete _pimpl->_batteryMgr;
};

void AppContext::begin()
{
    auto orientationMgr = OrientationManager(APP_ROTATION_THRESHOLD);
    orientationMgr.begin();
    _pimpl->_orientationMgr = &orientationMgr;
    auto settingsMgr = AppSettingsManager(EEPROM_SIZE, MAX_SETTINGS_NR);
    settingsMgr.begin();
    _pimpl->_appSettingsMgr = &settingsMgr;
    auto wifi = WifiManager();
    _pimpl->_wifiMgr = &wifi;
    auto vmix = VmixManager();
    _pimpl->_vmixMgr = &vmix;
    auto batt = BatteryManager();
    _pimpl->_batteryMgr = &batt;
}

bool AppContext::getOrientation()
{
    return _pimpl->_orientationMgr->getOrientation();
}

unsigned short AppContext::getSettingsIdx()
{
    return _pimpl->_appSettingsIdx;
}

unsigned short AppContext::getNumSettings()
{
    return _pimpl->_appSettingsMgr->getNumSettings();
}

AppSettingsManager* AppContext::getSettingsManager()
{
    return _pimpl->_appSettingsMgr;
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

bool AppContext::getIsWifiConnected()
{
    return _pimpl->_isWifiConnected;
}

void AppContext::setIsWifiConnected(bool connected)
{
    _pimpl->_isWifiConnected = connected;
}

VmixManager *AppContext::getVmixManager()
{
    return _pimpl->_vmixMgr;
}

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

unsigned int AppContext::getNumReconnections()
{
    return _pimpl->_numReconnections;
}

void AppContext::incNumReconnections()
{
    _pimpl->_numReconnections++;
}

bool AppContext::getIsCharging()
{
    return _pimpl->_isCharging;
}

void AppContext::setIsCharging(bool charging)
{
    _pimpl->_isCharging = charging;
}

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

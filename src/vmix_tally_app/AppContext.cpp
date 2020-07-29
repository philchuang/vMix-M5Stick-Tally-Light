#define ESP32

#include "AppContext.h"
#include <stdio.h>
#include <string.h>
#include <M5StickC.h>
#include "AppSettings.h"
#include "WifiManager.h"
#include "VmixManager.h"

struct AppContext::Impl
{
    Impl()
    {
    }

    ~Impl()
    {
    }

    AppSettings *_appSettings;
    bool _isWifiConnected;
    bool _isVmixConnected;
    char _tallyState;
    unsigned int _numReconnections;
    bool _isCharging;
    double _batteryLevel;
    WifiManager *_wifiMgr;
    VmixManager *_vmixMgr;
};

AppContext::AppContext()
    : _pimpl(new Impl())
{
}

AppContext::~AppContext() = default;

void AppContext::begin()
{
    auto wifi = WifiManager();
    _pimpl->_wifiMgr = &wifi;
    auto vmix = VmixManager();
    _pimpl->_vmixMgr = &vmix;
}

AppSettings *AppContext::getSettings()
{
    return _pimpl->_appSettings;
}

void AppContext::setSettings(AppSettings &settings)
{
    _pimpl->_appSettings = &settings;
}

WifiManager* AppContext::getWifiManager()
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

VmixManager* AppContext::getVmixManager()
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
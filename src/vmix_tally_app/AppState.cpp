#define ESP32

#include "AppState.h"
#include "AppSettings.h"
#include <stdio.h>
#include <string.h>
#include <M5StickC.h>

struct AppState::Impl
{
    Impl()
    {
    }

    ~Impl()
    {
    }

    AppSettings _appSettings;
    char _tallyState;
    unsigned int _numReconnections;
    bool _isCharging;
    double _batteryLevel;
};

AppState::AppState()
    : _pimpl(new Impl())
{
}

AppState::~AppState() = default;

void AppState::begin()
{
    // TODO implement
}

AppSettings AppState::getSettings()
{
    return _pimpl->_appSettings;
}

void AppState::setSettings(AppSettings settings)
{
    _pimpl->_appSettings = settings;
}

char AppState::getTallyState()
{
    return _pimpl->_tallyState;
}

void AppState::setTallyState(char state)
{
    _pimpl->_tallyState = state;
}

unsigned int AppState::getNumReconnections()
{
    return _pimpl->_numReconnections;
}

void AppState::incNumReconnections()
{
    _pimpl->_numReconnections++;
}

bool AppState::getIsCharging()
{
    return _pimpl->_isCharging;
}

void AppState::setIsCharging(bool charging)
{
    _pimpl->_isCharging = charging;
}

double AppState::getBatteryLevel()
{
    return _pimpl->_batteryLevel;
}

void AppState::setBatteryLevel(double battery)
{
    _pimpl->_batteryLevel = battery;
}
#include "AppContext.h"

// hardware
#define ESP32
#include <Arduino.h>
#include <M5StickC.h>

// libraries
#include <stdio.h>
#include <vector>
#include <string>
#include <Callback.h>

// app
#include "AppSettings.h"
#include "AppSettingsDefaults.h"
#include "AppSettingsManager.h"
#include "BatteryManager.h"
#include "Constants.h"
#include "Configuration.h"
#include "Screen.h"
#include "SlotLoopEvent.h"
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
    Impl(AppContext *appContext) : _checkIsChargingDelegate(appContext, &AppContext::checkIsCharging),
                                   _checkIsChargingTimer(_checkIsChargingDelegate, M5_CHARGING_MS),
                                   _checkBatteryLevelDelegate(appContext, &AppContext::checkBatteryLevel),
                                   _checkBatteryLevelTimer(_checkBatteryLevelDelegate, M5_BATTERYLEVEL_MS),
                                   _checkVmixResponseDelegate(appContext, &AppContext::checkVmixResponse),
                                   _checkVmixResponseTimer(_checkVmixResponseDelegate, VMIX_RESPONSE_MS),
                                   _checkVmixConnectionDelegate(appContext, &AppContext::checkVmixConnection),
                                   _checkVmixConnectionTimer(_checkVmixConnectionDelegate, VMIX_KEEPALIVE_MS),
                                   _isWifiConnected(false),
                                   _isVmixConnected(false),
                                   _isCharging(false),
                                   _isErrorFatal(false)
    {
    }

    ~Impl()
    {
    }

    AppSettingsManager _appSettingsMgr = AppSettingsManager(EEPROM_SIZE, MAX_SETTINGS_NR);
    unsigned short _appSettingsIdx;
    AppSettings _appSettings;

    unsigned short _currentScreen;

    WifiManager _wifiMgr;
    bool _isWifiConnected;
    unsigned int _numReconnections;

    VmixManager _vmixMgr;
    bool _isVmixConnected;
    char _tallyState;

    BatteryManager _batteryMgr;
    bool _isCharging;
    double _batteryLevel;

    std::vector<SlotLoopEvent *> _loopEvents;
    MethodSlot<AppContext, unsigned long> _checkIsChargingDelegate;
    SlotLoopEvent _checkIsChargingTimer;
    MethodSlot<AppContext, unsigned long> _checkBatteryLevelDelegate;
    SlotLoopEvent _checkBatteryLevelTimer;
    MethodSlot<AppContext, unsigned long> _checkVmixResponseDelegate;
    SlotLoopEvent _checkVmixResponseTimer;
    MethodSlot<AppContext, unsigned long> _checkVmixConnectionDelegate;
    SlotLoopEvent _checkVmixConnectionTimer;

    bool _isErrorFatal;

    void printSettingsDebug()
    {
        Serial.printf("SSID : %s\n", _appSettings.getWifiSsid());
        Serial.printf("Pass : %s\n", _appSettings.getWifiPassphrase());
        Serial.printf("vMix : %s\n", _appSettings.getVmixAddressWithPort());
        Serial.printf("Tally: %d\n", _appSettings.getVmixTally());
    }
};

AppContext::AppContext()
    : _pimpl(new Impl(this))
{
}

AppContext::~AppContext()
{
    this->sendScreenChange.~Signal();
    _pimpl->_appSettingsMgr.~AppSettingsManager();
    _pimpl->_wifiMgr.~WifiManager();
    _pimpl->_vmixMgr.~VmixManager();
    _pimpl->_batteryMgr.~BatteryManager();
};

void AppContext::begin()
{
    _pimpl->_appSettingsMgr.begin();

    _pimpl->_wifiMgr.begin();

    _pimpl->_vmixMgr.begin();

    _pimpl->_batteryMgr.begin();

    // set up polling events
    _pimpl->_loopEvents.push_back(&_pimpl->_checkIsChargingTimer);
    _pimpl->_loopEvents.push_back(&_pimpl->_checkBatteryLevelTimer);
    _pimpl->_loopEvents.push_back(&_pimpl->_checkVmixResponseTimer);
    _pimpl->_loopEvents.push_back(&_pimpl->_checkVmixConnectionTimer);
}

AppSettingsManager *AppContext::getSettingsManager()
{
    return &(_pimpl->_appSettingsMgr);
}

unsigned short AppContext::getSettingsIdx()
{
    return _pimpl->_appSettingsIdx;
}

unsigned short AppContext::getNumSettings()
{
    return _pimpl->_appSettingsMgr.getNumSettings();
}

AppSettings *AppContext::getSettings()
{
    return &_pimpl->_appSettings;
}

AppSettings *AppContext::loadSettings(unsigned short settingsIdx)
{
    Serial.printf("Loading settings at %d...\n", settingsIdx);
    _pimpl->_appSettings = _pimpl->_appSettingsMgr.load(settingsIdx);
    if (!_pimpl->_appSettings.isValid())
    {
        Serial.println("Invalid settings found, using default settings.");
        if (settingsIdx == 0)
        {
            _pimpl->_appSettings.setWifiSsid(SETTINGS0_WIFI_SSID);
            _pimpl->_appSettings.setWifiPassphrase(SETTINGS0_WIFI_PASS);
            _pimpl->_appSettings.setVmixAddress(SETTINGS0_VMIX_ADDR);
            _pimpl->_appSettings.setVmixPort(SETTINGS0_VMIX_PORT);
            _pimpl->_appSettings.setVmixTally(SETTINGS0_TALLY_NR);
        }
#ifdef SETTINGS1_WIFI_SSID
        else if (settingsIdx == 1)
        {
            _pimpl->_appSettings.setWifiSsid(SETTINGS1_WIFI_SSID);
            _pimpl->_appSettings.setWifiPassphrase(SETTINGS1_WIFI_PASS);
            _pimpl->_appSettings.setVmixAddress(SETTINGS1_VMIX_ADDR);
            _pimpl->_appSettings.setVmixPort(SETTINGS1_VMIX_PORT);
            _pimpl->_appSettings.setVmixTally(SETTINGS1_TALLY_NR);
        }
#endif
        _pimpl->_appSettingsMgr.save(settingsIdx, _pimpl->_appSettings);
        Serial.println("Default settings:");
        _pimpl->printSettingsDebug();
    }

    _pimpl->_appSettingsIdx = settingsIdx;
    return &_pimpl->_appSettings;
}

AppSettings *AppContext::cycleSettings()
{
    auto idx = (_pimpl->_appSettingsIdx + 1) % MAX_SETTINGS_NR;
    return this->loadSettings(idx);
}

unsigned short AppContext::getCurrentScreen()
{
    return _pimpl->_currentScreen;
}

void AppContext::setCurrentScreen(unsigned short currentScreen)
{
    _pimpl->_currentScreen = currentScreen;
}

WifiManager *AppContext::getWifiManager()
{
    return &(_pimpl->_wifiMgr);
}

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
    return &(_pimpl->_vmixMgr);
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

BatteryManager *AppContext::getBatteryManager()
{
    return &(_pimpl->_batteryMgr);
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
    return _pimpl->_batteryMgr.cycleBacklight();
}

void AppContext::setBacklight(unsigned int brightness)
{
    _pimpl->_batteryMgr.setBacklight(brightness);
}

void AppContext::checkIsCharging(unsigned long timestamp)
{
    this->setIsCharging(_pimpl->_batteryMgr.isCharging());
}

void AppContext::checkBatteryLevel(unsigned long timestamp)
{
    this->setBatteryLevel(_pimpl->_batteryMgr.getBatteryLevel());
    // if (LOG_BATTERY && _pimpl->_saveUptimeInfo)
    // {
    //     this->getSettingsManager()->saveUptimeInfo(millis(), this->getBatteryLevel());
    // }
}

void AppContext::addLoopEvent(SlotLoopEvent *event)
{
    _pimpl->_loopEvents.push_back(event);
}

void AppContext::handleLoop(unsigned long timestamp)
{
    for (auto it = _pimpl->_loopEvents.begin(); it != _pimpl->_loopEvents.end(); ++it)
    {
        (*it)->execute(timestamp);
    }
}

void AppContext::checkVmixResponse(unsigned long timestamp)
{
    if (_pimpl->_isVmixConnected)
    {
        _pimpl->_vmixMgr.receiveInput();
    }
}

void AppContext::checkVmixConnection(unsigned long timestamp)
{
    if (_pimpl->_currentScreen == SCREEN_ERROR ||
        _pimpl->_currentScreen == SCREEN_SPLASH ||
        _pimpl->_currentScreen == SCREEN_CONN ||
        (!_pimpl->_isWifiConnected && !_pimpl->_isVmixConnected) ||
        _pimpl->_isErrorFatal)
    {
        return;
    }

    if (!_pimpl->_wifiMgr.isAlive())
    {
        Serial.println("Disconnected from wifi, reconnecting...");
        this->sendScreenChange.fire(SCREEN_CONN);
    }

    if (!_pimpl->_vmixMgr.isAlive())
    {
        Serial.println("Disconnected from vMix, reconnecting...");
        _pimpl->_numReconnections++;
        this->sendScreenChange.fire(SCREEN_CONN);
    }
}

void AppContext::setErrorFatal()
{
    _pimpl->_isErrorFatal = true;
}

void AppContext::clearErrorFatal()
{
    _pimpl->_isErrorFatal = false;
}
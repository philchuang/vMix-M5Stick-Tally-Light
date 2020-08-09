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
#include "OrientationManager.h"
#include "Screen.h"
#include "SlotLoopEvent.h"
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
    std::vector<SlotLoopEvent> _loopEvents;

    void printSettingsDebug()
    {
        Serial.printf("SSID : %s\n", _appSettings->getWifiSsid());
        Serial.printf("Pass : %s\n", _appSettings->getWifiPassphrase());
        Serial.printf("vMix : %s\n", _appSettings->getVmixAddressWithPort());
        Serial.printf("Tally: %d\n", _appSettings->getVmixTally());
    }
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

    // set up polling events
    MethodSlot<AppContext, unsigned long> isChargingChecker(this, &AppContext::checkIsCharging);
    auto temp = SlotLoopEvent(isChargingChecker, M5_CHARGING_MS);
    _pimpl->_loopEvents.push_back(temp);

    MethodSlot<AppContext, unsigned long> batteryLevelChecker(this, &AppContext::checkBatteryLevel);
    temp = SlotLoopEvent(batteryLevelChecker, M5_BATTERYLEVEL_MS);
    _pimpl->_loopEvents.push_back(temp);

    MethodSlot<AppContext, unsigned long> orientationChecker(this, &AppContext::checkOrientation);
    temp = SlotLoopEvent(orientationChecker, APP_ORIENTATION_MS);
    _pimpl->_loopEvents.push_back(temp);

    // loopEvents.push_back(LoopEvent(main_checkOrientation, APP_ORIENTATION_MS));
    // loopEvents.push_back(*screenRefreshCheck);
    // loopEvents.push_back(LoopEvent(main_handleButtons, 0));
    // loopEvents.push_back(LoopEvent(main_pollVmix, VMIX_RESPONSE_MS));
    // loopEvents.push_back(LoopEvent(main_pollKeepAlive, VMIX_KEEPALIVE_MS));
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
    Serial.printf("Loading settings at %d...\n", settingsIdx);
    auto settings = _pimpl->_appSettingsMgr->load(settingsIdx);
    if (!settings.isValid())
    {
        Serial.println("Invalid settings found, using default settings.");
        if (settingsIdx == 0)
        {
            settings.setWifiSsid(SETTINGS0_WIFI_SSID);
            settings.setWifiPassphrase(SETTINGS0_WIFI_PASS);
            settings.setVmixAddress(SETTINGS0_VMIX_ADDR);
            settings.setVmixPort(SETTINGS0_VMIX_PORT);
            settings.setVmixTally(SETTINGS0_TALLY_NR);
        }
#ifdef SETTINGS1_WIFI_SSID
        else if (settingsIdx == 1)
        {
            settings.setWifiSsid(SETTINGS1_WIFI_SSID);
            settings.setWifiPassphrase(SETTINGS1_WIFI_PASS);
            settings.setVmixAddress(SETTINGS1_VMIX_ADDR);
            settings.setVmixPort(SETTINGS1_VMIX_PORT);
            settings.setVmixTally(SETTINGS1_TALLY_NR);
        }
#endif
        _pimpl->_appSettingsMgr->save(settingsIdx, &settings);
        Serial.println("Default settings:");
        _pimpl->printSettingsDebug();
    }

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

void AppContext::checkIsCharging(unsigned long timestamp)
{
    this->setIsCharging(this->getIsCharging());
}

void AppContext::checkBatteryLevel(unsigned long timestamp)
{
    this->setBatteryLevel(this->getBatteryLevel());
    if (LOG_BATTERY && _pimpl->_saveUptimeInfo)
    {
        this->getSettingsManager()->saveUptimeInfo(millis(), this->getBatteryLevel());
    }
}

void AppContext::checkOrientation(unsigned long timestamp)
{
}

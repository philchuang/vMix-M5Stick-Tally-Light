#include "AppSettings.h"

// hardware
#define ESP32
#include <M5StickC.h>
#include <EEPROM.h>

// libraries
#include <stdio.h>
#include <string>

// app
#include "AppSettingsManager.h"

// TODO convert to using Preferences instead of EEPROM

struct AppSettingsManager::Impl
{
    Impl(unsigned short eepromSize, unsigned short numSettings) : _eepromSize(eepromSize), _numSettings(numSettings) { }

    ~Impl() { }

    unsigned short _eepromSize;
    unsigned short _numSettings;
};

AppSettingsManager::AppSettingsManager(unsigned short eepromSize, unsigned short numSettings)
    : _pimpl(new Impl(eepromSize, numSettings))
{
}

AppSettingsManager::~AppSettingsManager() = default;

void AppSettingsManager::begin()
{
    EEPROM.begin(_pimpl->_eepromSize);
}

AppSettings AppSettingsManager::load(unsigned short settingsIdx)
{
    unsigned long ptr = settingsIdx * _pimpl->_eepromSize;

    AppSettings settings = AppSettings();

    char *temp = new char[WifiSsidMaxLength];
    EEPROM.readString(ptr, temp, WifiSsidMaxLength);
    settings.setWifiSsid(temp);
    delete[] temp;
    ptr += WifiSsidMaxLength;

    temp = new char[WifiPassMaxLength];
    EEPROM.readString(ptr, temp, WifiPassMaxLength);
    settings.setWifiPassphrase(temp);
    ptr += WifiPassMaxLength;
    delete[] temp;

    temp = new char[VmixAddrMaxLength];
    EEPROM.readString(ptr, temp, VmixAddrMaxLength);
    settings.setVmixAddress(temp);
    ptr += VmixAddrMaxLength;
    delete[] temp;

    settings.setVmixPort(EEPROM.readUShort(ptr));
    ptr += 2;

    settings.setVmixTally(EEPROM.readUShort(ptr));
    ptr += 2;

    return settings;
}

void AppSettingsManager::save(unsigned short settingsIdx, AppSettings *settings)
{
    this->clear(settingsIdx);

    unsigned long ptr = settingsIdx * _pimpl->_eepromSize;

    EEPROM.writeString(ptr, settings->getWifiSsid());
    ptr += WifiSsidMaxLength;

    EEPROM.writeString(ptr, settings->getWifiPassphrase());
    ptr += WifiPassMaxLength;

    EEPROM.writeString(ptr, settings->getVmixAddress());
    ptr += VmixAddrMaxLength;

    EEPROM.writeUShort(ptr, settings->getVmixPort());
    ptr += 2;

    EEPROM.writeUShort(ptr, settings->getVmixTally());
    ptr += 2;

    EEPROM.commit();
}

void AppSettingsManager::clear(unsigned short settingsIdx)
{
    unsigned long ptr = settingsIdx * _pimpl->_eepromSize;

    for (int i = 0; i < (_pimpl->_eepromSize >> 3); i++)
    {
        EEPROM.writeULong64(ptr, 0);
        ptr += 8;
    }

    EEPROM.commit();
}

unsigned short AppSettingsManager::getNumSettings()
{
    return _pimpl->_numSettings;
}

// TEMPORARY

void AppSettingsManager::saveUptimeInfo(unsigned long uptime, double batteryLevel)
{
    // save at the top 8 bytes
    unsigned long ptr = _pimpl->_eepromSize - 12;
    EEPROM.writeULong(ptr, uptime);
    ptr += 4;
    EEPROM.writeUInt(ptr, (unsigned int) batteryLevel);
    unsigned int temp = EEPROM.readUInt(ptr);
    EEPROM.commit();
}

unsigned long AppSettingsManager::getLastUptime()
{
    unsigned long ptr = _pimpl->_eepromSize - 12;
    unsigned long uptime = EEPROM.readULong(ptr);
    return uptime;
}

double AppSettingsManager::getLastBatteryLevel()
{
    unsigned long ptr = _pimpl->_eepromSize - 12 + 4;
    unsigned int batteryLevel = EEPROM.readUInt(ptr);
    return (double) batteryLevel;
}
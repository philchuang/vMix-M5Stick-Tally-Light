#define ESP32

#include "AppSettings.h"
#include "AppSettingsManager.h"
#include <stdio.h>
#include <string.h>
#include <EEPROM.h>
#include <M5StickC.h>

// TODO convert to using Preferences instead of EEPROM

struct AppSettingsManager::Impl
{
    Impl(unsigned short eepromSize, unsigned short numSettings)
    {
        _EepromSize = eepromSize;
    }

    ~Impl()
    {
    }

    unsigned short _EepromSize;
    unsigned short _NumSettings;
};

AppSettingsManager::AppSettingsManager(unsigned short eepromSize, unsigned short numSettings)
    : _pimpl(new Impl(eepromSize, numSettings))
{
}

AppSettingsManager::~AppSettingsManager() = default;

void AppSettingsManager::begin()
{
  EEPROM.begin(_pimpl->_EepromSize);
}

AppSettings AppSettingsManager::load(unsigned short settingsIdx)
{
    unsigned long ptr = settingsIdx * _pimpl->_EepromSize;

    AppSettings settings = AppSettings();

    char *temp = new char[AppSettings_WifiSsidMaxLength];
    EEPROM.readString(ptr, temp, AppSettings_WifiSsidMaxLength);
    settings.setWifiSsid(temp);
    delete[] temp;
    ptr += AppSettings_WifiSsidMaxLength;

    temp = new char[AppSettings_WifiPassMaxLength];
    EEPROM.readString(ptr, temp, AppSettings_WifiPassMaxLength);
    settings.setWifiPassphrase(temp);
    ptr += AppSettings_WifiPassMaxLength;
    delete[] temp;

    temp = new char[AppSettings_VmixAddrMaxLength];
    EEPROM.readString(ptr, temp, AppSettings_VmixAddrMaxLength);
    settings.setVmixAddress(temp);
    ptr += AppSettings_VmixAddrMaxLength;
    delete[] temp;

    settings.setVmixPort(EEPROM.readUShort(ptr));
    ptr += 2;

    settings.setVmixTally(EEPROM.readUShort(ptr));
    ptr += 2;

    return settings;
}

void AppSettingsManager::save(unsigned short settingsIdx, AppSettings settings)
{
    this->clear(settingsIdx);

    unsigned long ptr = settingsIdx * _pimpl->_EepromSize;

    EEPROM.writeString(ptr, settings.getWifiSsid());
    ptr += AppSettings_WifiSsidMaxLength;

    EEPROM.writeString(ptr, settings.getWifiPassphrase());
    ptr += AppSettings_WifiPassMaxLength;

    EEPROM.writeString(ptr, settings.getVmixAddress());
    ptr += AppSettings_VmixAddrMaxLength;

    EEPROM.writeUShort(ptr, settings.getVmixPort());
    ptr += 2;

    EEPROM.writeUShort(ptr, settings.getVmixTally());
    ptr += 2;

    EEPROM.commit();
}

void AppSettingsManager::clear(unsigned short settingsIdx)
{
    unsigned long ptr = settingsIdx * _pimpl->_EepromSize;

    for (int i = 0; i < (_pimpl->_EepromSize >> 3); i++)
    {
        EEPROM.writeULong64(ptr, 0);
        ptr += 8;
    }

    EEPROM.commit();
}
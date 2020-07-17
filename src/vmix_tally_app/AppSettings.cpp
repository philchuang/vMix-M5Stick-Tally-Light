#include "AppSettings.h"
#include <stdio.h>
#include <string.h>
#include <EEPROM.h>

#define WifiSsidMaxLength 64
#define WifiPassMaxLength 64
#define VmixAddrMaxLength 64
#define TallyNumberMaxValue 64

struct AppSettings::Impl
{
    Impl(unsigned short eepromSize)
    {
        _EepromSize = eepromSize;
    }

    ~Impl()
    {
    }

    unsigned short _EepromSize;
    char *_WifiSsid;
    char *_WifiPassphrase;
    char *_VmixAddress;
    unsigned short _VmixPort;
    unsigned short _VmixTally;
};

AppSettings::AppSettings(unsigned short eepromSize)
    : _pimpl(new Impl(eepromSize))
{
}

AppSettings::~AppSettings() = default;

char *AppSettings::getWifiSsid()
{
    return _pimpl->_WifiPassphrase;
}

void AppSettings::setWifiSsid(char *ssid)
{
    _pimpl->_WifiSsid = ssid;
}

char *AppSettings::getWifiPassphrase()
{
    return _pimpl->_WifiPassphrase;
}

void AppSettings::setWifiPassphrase(char *passphrase)
{
    _pimpl->_WifiPassphrase = passphrase;
}

char *AppSettings::getVmixAddress()
{
    return _pimpl->_VmixAddress;
}

void AppSettings::setVmixAddress(char *address)
{
    _pimpl->_VmixAddress = address;
}

unsigned short AppSettings::getVmixPort()
{
    return _pimpl->_VmixPort;
}

void AppSettings::setVmixPort(unsigned short port)
{
    _pimpl->_VmixPort = port;
}

unsigned short AppSettings::getVmixTally()
{
    return _pimpl->_VmixTally;
}

void AppSettings::setVmixTally(unsigned short tally)
{
    _pimpl->_VmixTally = tally;
}

char *AppSettings::getVmixAddressWithPort()
{
    char full[VmixAddrMaxLength + 1 + 5];
    sprintf(full, "%s:%u", _pimpl->_VmixAddress, _pimpl->_VmixPort);
    return full;
}

bool AppSettings::load()
{
    unsigned long ptr = 0;

    EEPROM.readString(ptr, _pimpl->_WifiSsid, WifiSsidMaxLength);
    ptr += WifiSsidMaxLength;

    EEPROM.readString(ptr, _pimpl->_WifiPassphrase, WifiPassMaxLength);
    ptr += WifiPassMaxLength;

    EEPROM.readString(ptr, _pimpl->_VmixAddress, VmixAddrMaxLength);
    ptr += VmixAddrMaxLength;

    _pimpl->_VmixPort = EEPROM.readUShort(ptr);
    ptr += 2;

    _pimpl->_VmixTally = EEPROM.readUShort(ptr);
    ptr += 2;

    if (strlen(_pimpl->_WifiSsid) == 0 || strlen(_pimpl->_WifiPassphrase) == 0 || strlen(_pimpl->_VmixAddress) == 0 || _pimpl->_VmixPort == 0u || _pimpl->_VmixTally == 0u)
    {
        return false;
    }

    return true;
}

void AppSettings::save()
{
    this->clear();

    unsigned long ptr = 0;

    EEPROM.writeString(ptr, _pimpl->_WifiSsid);
    ptr += WifiSsidMaxLength;

    EEPROM.writeString(ptr, _pimpl->_WifiPassphrase);
    ptr += WifiPassMaxLength;

    EEPROM.writeString(ptr, _pimpl->_VmixAddress);
    ptr += VmixAddrMaxLength;

    EEPROM.writeUShort(ptr, _pimpl->_VmixPort);
    ptr += 2;

    EEPROM.writeUShort(ptr, _pimpl->_VmixTally);
    ptr += 2;

    EEPROM.commit();
}

void AppSettings::clear()
{
    unsigned long ptr = 0;

    for (int i = 0; i < (_pimpl->_EepromSize >> 3); i++)
    {
        EEPROM.writeULong64(ptr, 0);
        ptr += 8;
    }

    EEPROM.commit();
}
#include "AppSettings.h"

// hardware
#include <M5StickC.h>
#include <EEPROM.h>

// libraries
#include <stdio.h>
#include <string>

struct AppSettings::Impl
{
    Impl() { }
    ~Impl() { }

    char *_WifiSsid = new char[WifiSsidMaxLength];
    char *_WifiPassphrase = new char[WifiPassMaxLength];
    char *_VmixAddress = new char[VmixAddrMaxLength];
    unsigned short _VmixPort;
    unsigned short _VmixTally;
};

AppSettings::AppSettings()
    : _pimpl(new Impl())
{
}

AppSettings::~AppSettings() = default;

char *AppSettings::getWifiSsid()
{
    return _pimpl->_WifiSsid;
}

void AppSettings::setWifiSsid(char *ssid)
{
    _pimpl->_WifiSsid = strdup(ssid);
}

char *AppSettings::getWifiPassphrase()
{
    return _pimpl->_WifiPassphrase;
}

void AppSettings::setWifiPassphrase(char *passphrase)
{
    _pimpl->_WifiPassphrase = strdup(passphrase);
}

char *AppSettings::getVmixAddress()
{
    return _pimpl->_VmixAddress;
}

void AppSettings::setVmixAddress(char *address)
{
    _pimpl->_VmixAddress = strdup(address);
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
    char *full = new char[VmixAddrMaxLength + 1 + 5];
    sprintf(full, "%s:%u", _pimpl->_VmixAddress, _pimpl->_VmixPort);
    return full;
}

bool AppSettings::isValid()
{
    if (strlen(_pimpl->_WifiSsid) == 0 || strlen(_pimpl->_WifiPassphrase) == 0 || strlen(_pimpl->_VmixAddress) == 0 || _pimpl->_VmixPort == 0u || _pimpl->_VmixTally == 0u)
    {
        return false;
    }

    return true;
}
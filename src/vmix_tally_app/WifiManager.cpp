#include "WifiManager.h"

// hardware
#define ESP32
#include <M5StickC.h>
#include <WiFi.h>

struct WifiManager::Impl
{
    Impl() { }

    ~Impl() { }
};

WifiManager::WifiManager() : _pimpl(new Impl())
{
}

WifiManager::~WifiManager() = default;

void WifiManager::begin()
{
    // anything here?
}

void WifiManager::connect(const char *ssid, const char *passphrase)
{
    // force a reconnection
    if (WiFi.status() == WL_CONNECTED)
    {
        WiFi.disconnect();
        delay(500);
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, passphrase);
}

bool WifiManager::isAlive()
{
    return WiFi.status() == WL_CONNECTED;
}

IPAddress WifiManager::localIP()
{
    return WiFi.localIP();
}

void WifiManager::disconnect()
{
    WiFi.disconnect();
}

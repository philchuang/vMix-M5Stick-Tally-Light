#define ESP32

#include "WifiManager.h"

#include <M5StickC.h>
#include <WiFi.h>
#include "AppState.h"

struct WifiManager::Impl
{
    Impl(AppState &state) : _state(&state)
    {
    }

    ~Impl()
    {
    }

    AppState *_state;
};

WifiManager::WifiManager(AppState &state)
    : _pimpl(new Impl(state))
{
}

WifiManager::~WifiManager()
{
}

void WifiManager::begin()
{
    // TODO anything go here?
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

#define ESP32

#define VMIX_API_SUBSCRIBE_TALLY "SUBSCRIBE TALLY\r\n"
#define VMIX_API_SUBSCRIBE_TALLY_RESPONSE_PREFIX "SUBSCRIBE OK TALLY Subscribed"
#define VMIX_API_GET_TALLY "TALLY\r\n"
#define VMIX_API_GET_TALLY_RESPONSE_PREFIX "TALLY OK "
#define VMIX_API_GET_VERSION_RESPONSE_PREFIX "VERSION OK "
#define VMIX_API_FUNCTION_QUICKPLAY_INPUT "FUNCTION QuickPlay Input=%d\r\n"
#define VMIX_API_FUNCTION_QUICKPLAY_INPUT_RESPONSE "FUNCTION OK COMPLETED"

#include "VmixManager.h"

#include <M5StickC.h>
#include "AppState.h"

struct VmixManager::Impl
{
    Impl(AppState &state) : _state(&state)
    {
    }

    ~Impl()
    {
    }

    AppState *_state;
};

VmixManager::VmixManager(AppState &state)
    : _pimpl(new Impl(state))
{
}

VmixManager::~VmixManager()
{
}

void VmixManager::begin()
{
    // TODO anything go here?
}

void VmixManager::connect(const char *ssid, const char *passphrase)
{
    // force a reconnection
    if (Vmix.status() == WL_CONNECTED)
    {
        Vmix.disconnect();
        delay(500);
    }

    Vmix.mode(Vmix_STA);
    Vmix.begin(ssid, passphrase);
}

bool VmixManager::isAlive()
{
    return Vmix.status() == WL_CONNECTED;
}

IPAddress VmixManager::localIP()
{
    return Vmix.localIP();
}

void VmixManager::disconnect()
{
    Vmix.disconnect();
}

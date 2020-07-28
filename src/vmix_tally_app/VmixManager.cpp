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
#include <WiFi.h>
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
    WiFiClient *_vmix_client;
};

VmixManager::VmixManager(AppState &state)
    : _pimpl(new Impl(state))
{
}

VmixManager::~VmixManager()
{
    _pimpl->_vmix_client = 0;
}

void VmixManager::begin()
{
    _pimpl->_vmix_client = &WiFiClient();
}

bool VmixManager::connect(const char *addr, unsigned short port)
{
    if (_pimpl->_vmix_client->connected())
    {
        _pimpl->_vmix_client->stop();
    }

    return _pimpl->_vmix_client->connect(addr, port);
}

bool VmixManager::isAlive()
{
    return _pimpl->_vmix_client->connected();
}

void VmixManager::disconnect()
{
    Vmix.disconnect();
}

void VmixManager::sendSubscribeTally()
{
    vmix_client.print(VMIX_API_SUBSCRIBE_TALLY);
}

void VmixManager::receive()
{
    // TODO
}
#include "VmixManager.h"

// hardware
#define ESP32
#include <M5StickC.h>
#include <WiFi.h>

// libraries
#include <string>

// constants
#define VMIX_API_SUBSCRIBE_TALLY "SUBSCRIBE TALLY\n"
#define VMIX_API_SUBSCRIBE_TALLY_RESPONSE_PREFIX "SUBSCRIBE OK TALLY Subscribed"
#define VMIX_API_GET_TALLY "TALLY\n"
#define VMIX_API_GET_TALLY_RESPONSE_PREFIX "TALLY OK "
#define VMIX_API_GET_VERSION_RESPONSE_PREFIX "VERSION OK "
#define VMIX_API_FUNCTION_QUICKPLAY_INPUT "FUNCTION QuickPlay Input=%d\n"
#define VMIX_API_FUNCTION_QUICKPLAY_INPUT_RESPONSE "FUNCTION OK COMPLETED"

struct VmixManager::Impl
{
    Impl() { }

    ~Impl() { }

    WiFiClient _vmix_client;
    String _lastTallyResponse;
    unsigned short _currentTally;
};

VmixManager::VmixManager() : _pimpl(new Impl()) { }

VmixManager::~VmixManager()
{
    _pimpl->_vmix_client.~WiFiClient();
}

void VmixManager::begin()
{
}

bool VmixManager::connect(const char *addr, unsigned short port)
{
    if (_pimpl->_vmix_client.connected())
    {
        _pimpl->_vmix_client.stop();
    }

    return _pimpl->_vmix_client.connect(addr, port);
}

bool VmixManager::isAlive()
{
    return _pimpl->_vmix_client.connected();
}

void VmixManager::disconnect()
{
    _pimpl->_vmix_client.stop();
}

void VmixManager::receiveInput()
{
    if (!_pimpl->_vmix_client.available())
    {
        return;
    }

    String data = _pimpl->_vmix_client.readStringUntil('\r\n');
    Serial.printf("VMIX recv: %s\n", data.c_str());

    if (data.indexOf(VMIX_API_GET_TALLY_RESPONSE_PREFIX) == 0)
    {
        auto trimmed = data.substring(8); // length of VMIX_API_GET_TALLY_RESPONSE_PREFIX
        auto changed = !trimmed.equals(_pimpl->_lastTallyResponse);
        if (changed)
        {
            _pimpl->_lastTallyResponse = trimmed;
            this->onTallyStateChange.fire(this->getCurrentTallyState());
        }
    }
    else if (data.indexOf(VMIX_API_GET_VERSION_RESPONSE_PREFIX) == 0)
    {
        Serial.println("vMix connection established.");
    }
    else if (data.indexOf(VMIX_API_SUBSCRIBE_TALLY_RESPONSE_PREFIX) == 0)
    {
        Serial.println("Tally subscription created.");
    }
    else if (data.indexOf(VMIX_API_FUNCTION_QUICKPLAY_INPUT_RESPONSE) == 0)
    {
        Serial.println("Function QuickPlay successful.");
    }
}

void VmixManager::sendSubscribeTally()
{
    if (!_pimpl->_vmix_client.connected())
        return;

    Serial.printf("VMIX send: %s\n", VMIX_API_SUBSCRIBE_TALLY);
    _pimpl->_vmix_client.printf(VMIX_API_SUBSCRIBE_TALLY);
}

void VmixManager::sendQuickPlayInput(unsigned short tallyNr)
{
    if (!_pimpl->_vmix_client.connected())
        return;

    Serial.printf("VMIX send: ");
    Serial.printf(VMIX_API_FUNCTION_QUICKPLAY_INPUT, tallyNr);
    Serial.println();
    _pimpl->_vmix_client.printf(VMIX_API_FUNCTION_QUICKPLAY_INPUT, tallyNr);
}

unsigned char VmixManager::getCurrentTallyState()
{
    if (_pimpl->_lastTallyResponse == 0 ||
        _pimpl->_currentTally == 0 ||
        _pimpl->_lastTallyResponse.length() - 1 <= _pimpl->_currentTally)
    {
        return TALLY_NONE;
    }

    char newState = _pimpl->_lastTallyResponse.charAt(_pimpl->_currentTally); // response is like "TALLY OK 00000000"
    Serial.printf("tally #%u state: %c\n", _pimpl->_currentTally, newState);

    return newState;
}

unsigned short VmixManager::getCurrentTallyNumber()
{
    return _pimpl->_currentTally;
}

void VmixManager::setCurrentTallyNumber(unsigned short tally)
{
    _pimpl->_currentTally = tally;
    this->onTallyStateChange.fire(this->getCurrentTallyState());
}
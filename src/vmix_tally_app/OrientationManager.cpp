#include "OrientationManager.h"

// hardware
#define ESP32
#include <M5StickC.h>
#include <AXP192.h>

struct OrientationManager::Impl
{
    Impl(float threshold) : _threshold(threshold) {}
    ~Impl() {}

    bool _isInitialized = false;
    bool _isPortrait = false;
    float _threshold = 0.0f;
};

OrientationManager::OrientationManager(float threshold)
    : _pimpl(new Impl(threshold)) {}

OrientationManager::~OrientationManager() = default;

void OrientationManager::begin()
{
    if (M5.MPU6886.Init() != 0)
    {
        Serial.println("MPU6886 error");
    }
    else
    {
        _pimpl->_isInitialized = true;
    }
}

unsigned int OrientationManager::checkRotationChange()
{
    return this->checkRotationChange(_pimpl->_threshold);
}

unsigned int OrientationManager::checkRotationChange(float threshold)
{
    if (!(_pimpl->_isInitialized))
    {
        return -1;
    }

    float accX = 0;
    float accY = 0;
    float accZ = 0;

    M5.MPU6886.getAccelData(&accX, &accY, &accZ);

    if (!(_pimpl->_isPortrait))
    {
        if (accX >= threshold)
        {
            return 1;
        }
        else if (accX <= -(threshold))
        {
            return 3;
        }
    }
    else
    {
        if (accY >= threshold)
        {
            return 0;
        }
        else if (accY <= -(threshold))
        {
            return 2;
        }
    }

    return -1;
}

bool OrientationManager::getOrientation()
{
    return _pimpl->_isPortrait;
}

void OrientationManager::setOrientation(bool isPortrait)
{
    _pimpl->_isPortrait = isPortrait;
}

short OrientationManager::getRotation()
{
    return M5.Lcd.getRotation();
}

void OrientationManager::setRotation(short rotation)
{
    if (this->getRotation() == rotation)
    {
        return;
    }

    Serial.printf("DEBUG: setRotation %d\n", rotation);
    M5.Lcd.setRotation(rotation);
}
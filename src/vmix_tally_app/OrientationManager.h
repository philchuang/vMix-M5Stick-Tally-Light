#define LANDSCAPE 0
#define PORTRAIT  1

#ifndef OrientationManager_h
#define OrientationManager_h

#define ESP32

// #include <Wire.h>
#include <M5StickC.h>
#include <AXP192.h>

class OrientationManager
{
public:
    OrientationManager(float threshold);
    ~OrientationManager();

    void begin();
    unsigned int checkRotationChange();
    unsigned int checkRotationChange(float threshold);
    bool getOrientation();
    void setOrientation(bool isPortrait);
    byte getRotation();
    void setRotation(byte rotation);

private:
    class Impl;
    Impl *_pimpl;
};

#endif
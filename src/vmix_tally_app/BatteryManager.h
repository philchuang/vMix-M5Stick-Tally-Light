#ifndef BATTERYMANAGER_H
#define BATTERYMANAGER_H

// hardware
#define ESP32
#include <M5StickC.h>
#include <AXP192.h>

class BatteryManager
{
public:
    BatteryManager();
    ~BatteryManager();
    
    void begin();

    double getBatteryLevel();
    bool isCharging();
    
    unsigned int cycleBacklight();
    void setBacklight(unsigned int brightness);
    
    void setLedOn(bool isOn);

private:
    class Impl;
    Impl *_pimpl;
};

#endif
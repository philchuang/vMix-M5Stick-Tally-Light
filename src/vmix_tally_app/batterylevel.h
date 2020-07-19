#ifndef BatteryLevel_h
#define BatteryLevel_h

#define ESP32

// #include <Wire.h>
#include <M5StickC.h>
#include <AXP192.h>

class BatteryLevel
{
public:
    BatteryLevel()
    {
        M5.Axp.begin();
    }
    ~BatteryLevel()
    {
    }
    double get()
    {
        // https://forum.m5stack.com/topic/1361/ischarging-and-getbatterylevel/7
        unsigned int vbatData = M5.Axp.GetVbatData();
        double vbat = vbatData * 1.1 / 1000;
        return 100.0 * ((vbat - 3.0) / (4.07 - 3.0));
    }
    bool isCharging()
    {
        return M5.Axp.GetInputPowerStatus() >> 2 > 0;
    }
private:
};

#endif
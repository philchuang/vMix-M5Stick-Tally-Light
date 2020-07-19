#ifndef BatteryLevel_h
#define BatteryLevel_h

#define ESP32
#define UNKNOWN_CONSTANT 1.1 // seriously what is this
#define MAX_WORKING_VOLTAGE 4.07
#define MIN_WORKING_VOLTAGE 3.07

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

        // impl #1
        /*
        unsigned int vbatData = M5.Axp.GetVbatData(); // docs: "Get the battery voltage value"
        double vbat = vbatData * 1.1 / 1000;
        // charging: 4.188 V
        // unplugged: 4.068 V (fully charged)
        // unplugged: 3.068 V (45 minutes later, about to turn off)

        unsigned int vapsData = M5.Axp.GetVapsData(); // docs: "Get battery capacity"
        double vaps = vapsData * 1.4 / 1000;
        // charging: 4.971 V
        // unplugged: 4.068 V (fully charged)
        // unplugged: 3.066 V (45 minutes later, about to turn off)

        double batteryLevel = 100.0 * ((vaps - 3.0) / (vbat - 3.0));
        M5.Lcd.printf("batteryLevel: %.0f%%    ", batteryLevel);
        */

       // TODO confirm minimum working voltage

        // impl #2
        unsigned int vbatData = M5.Axp.GetVbatData();
        double vbat = vbatData * UNKNOWN_CONSTANT / 1000;
        return 100.0 * ((vbat - MIN_WORKING_VOLTAGE) / (MAX_WORKING_VOLTAGE - MIN_WORKING_VOLTAGE));
    }
    bool isCharging()
    {
        return M5.Axp.GetInputPowerStatus() >> 2 > 0;
    }

private:
};

#endif
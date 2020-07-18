#ifndef BatteryLevel_h
#define BatteryLevel_h

// #include <Wire.h>
#include <M5StickC.h>
#include <AXP192.h>

class BatteryLevel
{
public:
    static double get()
    {
        // Wire.beginTransmission(0x75);
        // Wire.write(0x78);
        // if (Wire.endTransmission(false) == 0 && Wire.requestFrom(0x75, 1))
        // {
        //     switch (Wire.read() & 0xF0)
        //     {
        //     case 0xE0:
        //         return 25;
        //     case 0xC0:
        //         return 50;
        //     case 0x80:
        //         return 75;
        //     case 0x00:
        //         return 100;
        //     default:
        //         return 0;
        //     }
        //     //raw_battery = Wire.read();
        // }
        // return -1;
        unsigned int vbatData = M5.Axp.GetVbatData();
        double vbat = vbatData * 1.1 / 1000;
        return 100.0 * ((vbat - 3.0) / (4.07 - 3.0));
    }
private:
};

#endif
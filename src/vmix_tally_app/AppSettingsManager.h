/* AppSettingsManager.h - class that manages settings serialization for the vMixTallyApp.
 */

#ifndef APPSETTINGSMANAGER_H
#define APPSETTINGSMANAGER_H

// app
#include "AppSettings.h"

class AppSettingsManager
{
public:
    AppSettingsManager(unsigned short eepromSize, unsigned short numSettings);
    ~AppSettingsManager();
    
    void begin();
    
    AppSettings load(unsigned short settingsIdx);
    void save(unsigned short settingsIdx, AppSettings &settings);
    void clear(unsigned short settingsIdx);
    unsigned short getNumSettings();
    
    void saveLastUptimeInfo(unsigned long uptime, double batteryLevel);
    void saveUptimeInfo(unsigned long uptime, double batteryLevel);
    unsigned long getLastUptime();
    unsigned long getUptime();
    double getLastBatteryLevel();
    double getBatteryLevel();

private:
    class Impl;
    Impl *_pimpl;
};

#endif
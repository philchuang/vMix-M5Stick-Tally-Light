/* AppContext.h - class that contains application-wide state
 */

#ifndef APPCONTEXT_H
#define APPCONTEXT_H

#include "AppSettings.h"
#include "AppSettingsManager.h"
#include "BatteryManager.h"
#include "OrientationManager.h"
#include "VmixManager.h"
#include "WifiManager.h"

class AppContext
{
public:
    AppContext();
    ~AppContext();

    void begin();
    
    AppSettingsManager* getSettingsManager();
    unsigned short getSettingsIdx();
    unsigned short getNumSettings();
    AppSettings* getSettings();
    AppSettings* loadSettings(unsigned short settingsIdx);
    AppSettings* cycleSettings();
    // TODO saveSettings(AppSettings &settings);

    WifiManager* getWifiManager();
    bool getIsWifiConnected();
    void setIsWifiConnected(bool connected);

    unsigned int getNumReconnections();
    void incNumReconnections();

    VmixManager* getVmixManager();
    bool getIsVmixConnected();
    void setIsVmixConnected(bool connected);
    char getTallyState();
    void setTallyState(char state);

    OrientationManager* getOrientationManager();
    bool getOrientation();

    BatteryManager* getBatteryManager();
    bool getIsCharging();
    void setIsCharging(bool charging);
    double getBatteryLevel();
    void setBatteryLevel(double batt);
    unsigned int cycleBacklight();
    void setBrightness(unsigned int brightness);

    ScreenManager* getScreenManager();

private:
    class Impl;
    Impl *_pimpl;
};

#endif

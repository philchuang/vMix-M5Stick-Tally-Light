/* AppContext.h - class that contains application-wide state
 */

#ifndef APPCONTEXT_H
#define APPCONTEXT_H

#include "AppSettings.h"
#include "AppSettingsDefaults.h"
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

    AppSettingsManager *getSettingsManager();
    unsigned short getSettingsIdx();
    unsigned short getNumSettings();
    AppSettings *getSettings();
    AppSettings *loadSettings(unsigned short settingsIdx);
    AppSettings *cycleSettings();
    // TODO saveSettings(AppSettings &settings);

    WifiManager *getWifiManager();
    bool getIsWifiConnected();
    void setIsWifiConnected(bool connected);

    unsigned int getNumReconnections();
    void incNumReconnections();

    VmixManager *getVmixManager();
    bool getIsVmixConnected();
    void setIsVmixConnected(bool connected);
    char getTallyState();
    void setTallyState(char state);

    OrientationManager *getOrientationManager();
    bool getOrientation();

// TODO redo all these get/sets - differentiate between last saved state and shortcuts for manager methods
    BatteryManager *getBatteryManager();
    bool getIsCharging();
    double getBatteryLevel();
    unsigned int cycleBacklight();
    void setBacklight(unsigned int brightness);

    // ScreenManager* getScreenManager();

protected:
    void checkIsCharging(unsigned long timestamp)
    {
        this->setIsCharging(this->getIsCharging());
    }

    void setIsCharging(bool charging);

    void checkBatteryLevel(unsigned long timestamp)
    {
        this->setBatteryLevel(this->getBatteryLevel());
        if (LOG_BATTERY && saveUptimeInfo)
        {
            settingsMgr.saveUptimeInfo(millis(), currentBatteryLevel);
        }
    }

    void setBatteryLevel(double batt);

    void checkOrientation(unsigned long timestamp)
    {
    }

private:
    class Impl;
    Impl *_pimpl;
};

#endif

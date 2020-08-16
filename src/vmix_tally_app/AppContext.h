/* AppContext.h - class that contains application-wide state
 */

#ifndef APPCONTEXT_H
#define APPCONTEXT_H

#include "Callback.h"

#include "AppSettings.h"
#include "AppSettingsDefaults.h"
#include "AppSettingsManager.h"
#include "BatteryManager.h"
#include "Configuration.h"
#include "Constants.h"
#include "OrientationManager.h"
#include "SlotLoopEvent.h"
#include "VmixManager.h"
#include "WifiManager.h"

class AppContext
{
public:
    AppContext();
    ~AppContext();

    Signal<unsigned short> sendScreenChange;

    void begin();

    AppSettingsManager *getSettingsManager();
    // passthroughs
    unsigned short getNumSettings();
    // state accessors
    unsigned short getSettingsIdx();
    AppSettings *getSettings();
    // related functions
    AppSettings *loadSettings(unsigned short settingsIdx);
    AppSettings *cycleSettings();
    // TODO saveSettings(AppSettings &settings);

    WifiManager *getWifiManager();
    // state accessors
    bool getIsWifiConnected();
    void setIsWifiConnected(bool connected);

    // random state accessors
    unsigned int getNumReconnections();
    void incNumReconnections();

    VmixManager *getVmixManager();
    // state accessors
    bool getIsVmixConnected();
    void setIsVmixConnected(bool connected);
    char getTallyState();
    void setTallyState(char state);

    OrientationManager *getOrientationManager();
    // state accessors
    bool getOrientation();

    BatteryManager *getBatteryManager();
    // passthroughs
    unsigned int cycleBacklight();
    void setBacklight(unsigned int brightness);
    // state accessors
    bool getIsCharging();
    double getBatteryLevel();

    // loop functionality
    void addLoopEvent(SlotLoopEvent* event);
    void handleLoop(unsigned long timestamp);

    void setErrorFatal();
    void clearErrorFatal();

protected:

    // polling events
    void checkIsCharging(unsigned long timestamp);
    void checkBatteryLevel(unsigned long timestamp);
    void checkOrientation(unsigned long timestamp);
    void checkVmixResponse(unsigned long timestamp);
    void checkVmixConnection(unsigned long timestamp);
    // state accessors
    void setIsCharging(bool charging);
    void setBatteryLevel(double batt);

private:
    class Impl;
    Impl *_pimpl;
};

#endif

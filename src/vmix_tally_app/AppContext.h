/* AppContext.h - class that contains application-wide state
 */

#define TALLY_NONE '?'
#define TALLY_SAFE '0'
#define TALLY_LIVE '1'
#define TALLY_PRE  '2'

#ifndef AppContext_h
#define AppContext_h

#include "AppSettings.h"
#include "WifiManager.h"
#include "VmixManager.h"

class AppContext
{
public:
    AppContext();
    ~AppContext();

    void begin();
    
    AppSettings* getSettings();
    void setSettings(AppSettings &settings);

    WifiManager* getWifiManager();

    bool getIsWifiConnected();
    void setIsWifiConnected(bool connected);

    VmixManager* getVmixManager();

    bool getIsVmixConnected();
    void setIsVmixConnected(bool connected);
    
    char getTallyState();
    void setTallyState(char state);

    unsigned int getNumReconnections();
    void incNumReconnections();

    bool getIsCharging();
    void setIsCharging(bool charging);

    double getBatteryLevel();
    void setBatteryLevel(double batt);

private:
    class Impl;
    Impl *_pimpl;
};

#endif

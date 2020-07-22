/* AppState.h - class that contains application-wide state
 */

#ifndef AppState_h
#define AppState_h

#include "AppSettings.h"

class AppState
{
public:
    AppState();
    ~AppState();

    void begin();
    
    AppSettings getSettings();
    void setSettings(AppSettings settings);
    
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

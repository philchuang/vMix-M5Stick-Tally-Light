/* AppSettingsManager.h - class that manages settings serialization for the vMixTallyApp.
 */

#ifndef AppSettingsManager_h
#define AppSettingsManager_h

#include "AppSettings.h"

class AppSettingsManager
{
public:
    AppSettingsManager(unsigned short eepromSize, unsigned short numSettings);
    ~AppSettingsManager();
    AppSettings load(unsigned short settingsIdx);
    void save(unsigned short settingsIdx, AppSettings settings);
    void clear(unsigned short settingsIdx);
private:
    class Impl;
    Impl *_pimpl;
};

#endif
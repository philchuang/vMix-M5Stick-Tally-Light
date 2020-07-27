/* VmixManager.h - class that handles Vmix connections
 */

#ifndef VmixManager_h
#define VmixManager_h

#include <WiFi.h>
#include "AppState.h"

class VmixManager
{
public:
    VmixManager(AppState &state);
    ~VmixManager();

    void begin();
    void connect(const char *addr, unsigned short port);
    bool isAlive();
    void disconnect();

private:
    class Impl;
    Impl *_pimpl;
};

#endif

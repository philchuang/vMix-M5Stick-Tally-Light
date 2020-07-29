/* VmixManager.h - class that handles Vmix connections
 */

#ifndef VmixManager_h
#define VmixManager_h

#include <WiFi.h>

class VmixManager
{
public:
    VmixManager();
    ~VmixManager();

    void begin();
    bool connect(const char *addr, unsigned short port);
    bool isAlive();
    void disconnect();

    void sendSubscribeTally();
    void receiveInput();

private:
    class Impl;
    Impl *_pimpl;
};

#endif

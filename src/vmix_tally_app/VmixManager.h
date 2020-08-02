/* VmixManager.h - class that handles Vmix connections
 */

#define TALLY_NONE '?'
#define TALLY_SAFE '0'
#define TALLY_LIVE '1'
#define TALLY_PRE  '2'

#ifndef VmixManager_h
#define VmixManager_h

#include <WiFi.h>
#include <Callback.h>

class VmixManager
{
public:
    VmixManager();
    ~VmixManager();

    Signal<char> onTallyStateChange;

    void begin();
    bool connect(const char *addr, unsigned short port);
    bool isAlive();
    void disconnect();
    
    void receiveInput();

    void sendSubscribeTally();
    void sendQuickPlayInput(unsigned short tallyNr);

    unsigned char getCurrentTallyState();

    unsigned short getCurrentTallyNumber();
    void setCurrentTallyNumber(unsigned short tallyNr);

private:
    class Impl;
    Impl *_pimpl;
};

#endif

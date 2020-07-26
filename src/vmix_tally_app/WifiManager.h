/* WifiManager.h - class that handles wifi connections
 */

#ifndef WifiManager_h
#define WifiManager_h

#include "AppState.h"
#include <IPAddress.h>

class WifiManager
{
public:
    WifiManager(AppState &state);
    ~WifiManager();

    void begin();
    void connect(const char *ssid, const char *passphrase);
    bool isAlive();
    IPAddress localIP();
    void disconnect();

private:
    class Impl;
    Impl *_pimpl;
};

#endif

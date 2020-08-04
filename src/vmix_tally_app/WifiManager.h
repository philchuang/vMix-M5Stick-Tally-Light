/* WifiManager.h - class that handles wifi connections
 */

#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

// libraries
#include <IPAddress.h>

class WifiManager
{
public:
    WifiManager();
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

/* AppSettings.h - class that contains settings and other persisted data for the vMixTallyApp.
 */

// constants
#define WifiSsidMaxLength 64
#define WifiPassMaxLength 64
#define VmixAddrMaxLength 64
#define AppSettingsSize 200

#ifndef APPSETTINGS_H
#define APPSETTINGS_H

class AppSettings
{
public:
    AppSettings();
    ~AppSettings();

    char *getWifiSsid();
    void setWifiSsid(char *ssid);

    char *getWifiPassphrase();
    void setWifiPassphrase(char *passphrase);

    char *getVmixAddress();
    void setVmixAddress(char *address);

    unsigned short getVmixPort();
    void setVmixPort(unsigned short port);

    unsigned short getVmixTally();
    void setVmixTally(unsigned short tally);

    char *getVmixAddressWithPort();
    
    bool isValid();

private:
    class Impl;
    Impl *_pimpl;
};

#endif

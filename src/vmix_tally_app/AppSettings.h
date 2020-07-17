/* AppSettings.h - class that contains settings and other persisted data for the vMixTallyApp.
 */

#ifndef AppSettings_h
#define AppSettings_h

#define AppSettings_WifiSsidMaxLength 64
#define AppSettings_WifiPassMaxLength 64
#define AppSettings_VmixAddrMaxLength 64
#define AppSettings_Size 200

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

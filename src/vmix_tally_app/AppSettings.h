/* AppSettings.h - class that contains settings and other persisted data for the vMixTallyApp.
 */

#ifndef AppSettings_h
#define AppSettings_h

class AppSettings
{
public:
    AppSettings(unsigned short eepromSize);
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
    bool load();
    void save();
    void clear();

private:
    class Impl;
    Impl *_pimpl;
};

#endif

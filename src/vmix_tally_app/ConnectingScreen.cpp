// hardware
#define ESP32
#include <Arduino.h>
#include <M5StickC.h>

// libraries
#include <PinButton.h>

// app
#include "AppContext.h"
#include "Screen.h"
#include "VmixManager.h"
#include "WifiManager.h"

// constants
#define NUM_WAITS 10
#define WAIT_INTERVAL_MS 1000
#define WIFI_ERROR "Could not connect to wifi!"
#define VMIX_ERROR "Could not connect to vMix!"

class ConnectingScreen : public Screen
{
public:
    ConnectingScreen(AppContext &context) : Screen(this, context)
    {
        this->_wifiMgr = _context->getWifiManager();
        this->_vmixMgr = _context->getVmixManager();
    }

    ~ConnectingScreen()
    {
        delete this->_wifiMgr;
        delete this->_vmixMgr;
    }

    unsigned int getId() { return SCREEN_CONN; }

    void show()
    {
        M5.Lcd.fillScreen(TFT_BLACK);

        if (!reconnectWifi())
        {
            this->sendShowFatalErrorScreen.fire(WIFI_ERROR);
            return;
        }

        if (!reconnectVmix())
        {
            this->sendShowFatalErrorScreen.fire(VMIX_ERROR);
            return;
        }

        this->sendScreenChange.fire(SCREEN_TALLY);
    }

    void refresh()
    {
        // does nothing
    }

    void handleInput(unsigned long timestamp, PinButton &m5Btn, PinButton &sideBtn)
    {
        // does nothing
    }

private:
    bool reconnectWifi()
    {
        this->_context->setIsWifiConnected(false);
        this->_context->setIsVmixConnected(false);

        auto settings = this->_context->getSettings();

        const char *ssid = settings->getWifiSsid();
        const char *passphrase = settings->getWifiPassphrase();

        Serial.printf("SSID: %s\n", ssid);
        Serial.printf("Pass: %s\n", passphrase);
        Serial.printf("Connecting to %s with %s...", ssid, passphrase);

        this->sendOrientationChange.fire(LANDSCAPE);
        this->sendColorChange.fire(Colors(TFT_WHITE, TFT_BLACK));
        M5.Lcd.fillScreen(TFT_BLACK);
        M5.Lcd.setCursor(0, 0);
        M5.Lcd.setTextSize(1);
        M5.Lcd.print("Connecting to WiFi...");

        this->_wifiMgr->connect(ssid, passphrase);

        byte timeout = NUM_WAITS;
        while (!this->_wifiMgr->isAlive() && timeout > 0)
        {
            delay(WAIT_INTERVAL_MS);
            timeout--;
            Serial.print(".");
            M5.Lcd.print(".");
        }
        Serial.println();
        M5.Lcd.println();

        if (this->_wifiMgr->isAlive())
        {
            this->_context->setIsWifiConnected(true);

            Serial.println("Connected!");
            M5.Lcd.println("Connected!");
            Serial.print("IP: ");
            Serial.println(this->_wifiMgr->localIP());
            return true;
        }

        Serial.printf("Unable to connect to %s!\n", ssid);
        M5.Lcd.printf("Unable to connect to %s!\n", ssid);

        return false;
    }

    bool reconnectVmix()
    {
        this->_context->setIsVmixConnected(false);
        this->_context->setTallyState(TALLY_NONE);

        auto settings = this->_context->getSettings();

        if (!this->_context->getIsWifiConnected())
        {
            if (!reconnectWifi())
            {
                return false;
            }
        }

        this->sendOrientationChange.fire(LANDSCAPE);
        this->sendColorChange.fire(Colors(TFT_WHITE, TFT_BLACK));
        M5.Lcd.fillScreen(TFT_BLACK);
        M5.Lcd.setCursor(0, 0);
        M5.Lcd.setTextSize(1);
        Serial.println("Connecting to vMix...");
        M5.Lcd.println("Connecting to vMix...");

        auto addr = this->_context->getSettings()->getVmixAddress();
        auto port = this->_context->getSettings()->getVmixPort();

        byte timeout = NUM_WAITS;
        while (!this->_vmixMgr->connect(addr, port) && timeout > 0)
        {
            delay(WAIT_INTERVAL_MS);
            timeout--;
            Serial.print(".");
            M5.Lcd.print(".");
        }
        Serial.println();
        M5.Lcd.println();

        if (!this->_vmixMgr->isAlive())
        {
            Serial.println("Unable to connect to vMix!");
            M5.Lcd.println("Unable to connect to vMix!");
            return false;
        }

        this->_context->setIsVmixConnected(true);

        Serial.println("Connection opened.");
        Serial.println("Subscribing to tally events...");
        this->_vmixMgr->sendSubscribeTally();

        return true;
    }

    // local pointers to connection state
    WifiManager *_wifiMgr;
    VmixManager *_vmixMgr;
};
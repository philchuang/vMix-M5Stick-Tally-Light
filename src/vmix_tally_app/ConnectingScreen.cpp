#ifndef CONNSCREEN_CPP
#define CONNSCREEN_CPP

#define NUM_WAITS 10
#define WAIT_INTERVAL_MS 1000

#define WIFI_ERROR "Could not connect to wifi!"
#define VMIX_ERROR "Could not connect to vMix!"

#define ESP32

#include "Screen.h"

#include <M5StickC.h>
#include <PinButton.h>
#include "OrientationManager.h"
#include "AppContext.h"
#include "WifiManager.h"
#include "VmixManager.h"

class ConnectingScreen : public Screen
{
public:
    ConnectingScreen(AppContext &context) : Screen(context)
    {
        this->_wifiMgr = _context->getWifiManager();
        this->_vmixMgr = _context->getVmixManager();
    }

    ~ConnectingScreen()
    {
        this->_wifiMgr = 0;
        this->_vmixMgr = 0;
    }

    unsigned int getId() { return SCREEN_CONN; }

    void setOrientation(unsigned short orientation)
    {
        // does nothing, always landscape
    }

    void show()
    {
        M5.Lcd.fillScreen(TFT_BLACK);

        if (!reconnectWifi())
        {
            this->_errorMessage = WIFI_ERROR;
            refresh();
            return;
        }

        if (!reconnectVmix())
        {
            this->_errorMessage = VMIX_ERROR;
            refresh();
            return;
        }

        if (this->screenChangeHandler != 0)
        {
            this->screenChangeHandler(SCREEN_TALLY);
        }
    }

    void refresh()
    {
        if (this->_errorMessage != 0)
        {
            M5.Lcd.fillScreen(TFT_BLACK);

            if (this->orientationChangeHandler)
                this->orientationChangeHandler(LANDSCAPE);
            if (this->colorChangeHandler)
                this->colorChangeHandler(TFT_WHITE, TFT_BLACK);

            // display errormessage
            M5.Lcd.setCursor(0, 0);
            M5.Lcd.setTextSize(1);
            M5.Lcd.println(this->_errorMessage);
            
            M5.Lcd.println();
            
            // display settings
            auto settings = this->_context->getSettings();
            auto settingsIdx = this->_context->getSettingsIdx();
            auto numSettings = this->_context->getNumSettings();
            M5.Lcd.printf("SETTINGS: %d/%d\n", settingsIdx + 1, numSettings);
            M5.Lcd.printf("-SSID: %s\n", settings->getWifiSsid());
            M5.Lcd.printf("-IP: ");
            M5.Lcd.println(this->_wifiMgr->localIP());
            M5.Lcd.printf("-vMix: %s\n", settings->getVmixAddressWithPort());

            M5.Lcd.println();

            // display instructions
            M5.Lcd.println("Press M5 button to retry.");
            M5.Lcd.println("Press side button to switch settings.");
        }
    }

    void handleInput(unsigned long timestamp, PinButton m5Btn, PinButton sideBtn)
    {
        if (this->_errorMessage == 0)
            return;

        // m5 button single click to retry
        if (m5Btn.isSingleClick())
        {
            restart();
            return;
        }

        // side button single click to switch settings
        else if (sideBtn.isSingleClick())
        {
            this->_context->cycleSettings();
            restart();
            return;
        }
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

        if (this->orientationChangeHandler)
            this->orientationChangeHandler(LANDSCAPE);
        if (this->colorChangeHandler)
            this->colorChangeHandler(TFT_WHITE, TFT_BLACK);
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

        if (this->orientationChangeHandler)
            this->orientationChangeHandler(LANDSCAPE);
        if (this->colorChangeHandler)
            this->colorChangeHandler(TFT_WHITE, TFT_BLACK);
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

    void settings_swap()
    {
    }

    void restart()
    {
        Serial.println();
        Serial.println();
        Serial.println("------------");
        Serial.println("RESTART");
        Serial.println("------------");
        Serial.println();
        Serial.println();

        show();
    }

    char *_errorMessage = 0;
    // local pointers to connection state
    WifiManager *_wifiMgr;
    VmixManager *_vmixMgr;
};

#endif
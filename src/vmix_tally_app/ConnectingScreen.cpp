// hardware
#define ESP32
#include <Arduino.h>
#include <M5StickC.h>

// libraries
#include <memory>
#include <PinButton.h>

// app
#include "AppContext.h"
#include "Screen.h"
#include "VmixManager.h"
#include "WifiManager.h"

// constants
#define NUM_RETRIES 10
#define WAIT_INTERVAL_MS 1000u
#define WIFI_ERROR "Could not connect to wifi!"
#define VMIX_ERROR "Could not connect to vMix!"

class ConnectingScreen : public Screen
{
public:
    ConnectingScreen(AppContext &context) : Screen(this, context),
                                            _settings(context.getSettings()),
                                            _pollReconnectWifiDelegate(this, &ConnectingScreen::pollReconnectWifi),
                                            _pollReconnectWifiTimer(_pollReconnectWifiDelegate, WAIT_INTERVAL_MS),
                                            _pollReconnectVmixDelegate(this, &ConnectingScreen::pollReconnectVmix),
                                            _pollReconnectVmixTimer(_pollReconnectVmixDelegate, WAIT_INTERVAL_MS)
    {
        this->_wifiMgr = _context->getWifiManager();
        this->_vmixMgr = _context->getVmixManager();
    }

    ~ConnectingScreen()
    {
        delete this->_settings;
        delete this->_wifiMgr;
        delete this->_vmixMgr;
    }

    unsigned int getId() { return SCREEN_CONN; }

    void show()
    {
        this->sendOrientationChange.fire(LANDSCAPE);
        M5.Lcd.fillScreen(TFT_BLACK);

        if (!this->_isConnectingWifi && !this->_wifiMgr->isAlive())
        {
            showConnectingWifiScreen();
            return;
        }

        if (!this->_isConnectingVmix && !this->_vmixMgr->isAlive())
        {
            showConnectingVmixScreen();
            return;
        }

        if (this->_wifiMgr->isAlive() && this->_vmixMgr->isAlive())
        {
            this->sendScreenChange.fire(SCREEN_TALLY);
            return;
        }
    }

    void refresh()
    {
        // does nothing
    }

    void handleInput(unsigned long timestamp, PinButton &m5Btn, PinButton &sideBtn)
    {
        if (this->_isConnectingWifi)
        {
            this->_pollReconnectWifiTimer.execute(timestamp);
        }
        else if (this->_isConnectingVmix)
        {
            this->_pollReconnectVmixTimer.execute(timestamp);
        }
    }

private:
    bool _isConnectingWifi = false;
    bool _isConnectingVmix = false;
    int _numRetries = 0;
    AppSettings *_settings;

    MethodSlot<ConnectingScreen, unsigned long> _pollReconnectWifiDelegate;
    SlotLoopEvent _pollReconnectWifiTimer;
    MethodSlot<ConnectingScreen, unsigned long> _pollReconnectVmixDelegate;
    SlotLoopEvent _pollReconnectVmixTimer;

    WifiManager *_wifiMgr;
    VmixManager *_vmixMgr;

    void showConnectingWifiScreen()
    {
        this->_context->setIsWifiConnected(false);
        this->_context->setIsVmixConnected(false);
        this->_numRetries = 0;

        const char *ssid = this->_settings->getWifiSsid();
        const char *passphrase = this->_settings->getWifiPassphrase();

        this->sendOrientationChange.fire(LANDSCAPE);
        this->sendColorChange.fire(Colors(TFT_WHITE, TFT_BLACK));
        M5.Lcd.fillScreen(TFT_BLACK);
        M5.Lcd.setCursor(0, 0);
        M5.Lcd.setTextSize(1);
        M5.Lcd.print("Connecting to WiFi...");

        Serial.printf("SSID: %s\n", ssid);
        Serial.printf("Pass: %s\n", passphrase);
        Serial.printf("Connecting to %s with %s...", ssid, passphrase);

        this->_wifiMgr->connect(ssid, passphrase);
        this->_pollReconnectWifiTimer.setNextExecute(millis());
        this->_isConnectingWifi = true;
    }

    void showConnectingVmixScreen()
    {
        this->_context->setIsVmixConnected(false);
        this->_context->setTallyState(TALLY_NONE);
        this->_numRetries = 0;

        if (!this->_wifiMgr->isAlive())
        {
            this->_isConnectingWifi = false;
            this->show();
            return;
        }

        auto addr = this->_settings->getVmixAddress();
        auto port = this->_settings->getVmixPort();

        this->sendOrientationChange.fire(LANDSCAPE);
        this->sendColorChange.fire(Colors(TFT_WHITE, TFT_BLACK));
        M5.Lcd.fillScreen(TFT_BLACK);
        M5.Lcd.setCursor(0, 0);
        M5.Lcd.setTextSize(1);
        M5.Lcd.print("Connecting to vMix...");

        Serial.printf("Connecting to vMix at %s:%d...", addr, port);

        this->_vmixMgr->connect(addr, port);
        this->_pollReconnectVmixTimer.setNextExecute(millis());
        this->_isConnectingVmix = true;
    }

    void pollReconnectWifi(unsigned long timestamp)
    {
        if (!this->_isConnectingWifi)
            return;

        if (!this->_wifiMgr->isAlive())
        {
            this->_numRetries++;
            if (this->_numRetries == NUM_RETRIES)
            {
                Serial.println();
                M5.Lcd.println();
                this->_isConnectingWifi = false;
                this->sendShowFatalErrorScreen.fire("Unable to connect to wifi!");
                return;
            }
            Serial.print(".");
            M5.Lcd.print(".");
            this->_pollReconnectWifiTimer.setNextExecute(timestamp + WAIT_INTERVAL_MS);
            return;
        }
        Serial.println();
        M5.Lcd.println();

        Serial.println("Connected!");
        M5.Lcd.println("Connected!");
        Serial.print("IP: ");
        Serial.println(this->_wifiMgr->localIP());

        this->_context->setIsWifiConnected(true);
        this->_isConnectingWifi = false;
        this->show();
    }

    void pollReconnectVmix(unsigned long timestamp)
    {
        if (!this->_isConnectingVmix)
            return;

        if (!this->_vmixMgr->isAlive())
        {
            this->_numRetries++;
            if (this->_numRetries == NUM_RETRIES)
            {
                Serial.println();
                M5.Lcd.println();
                this->_isConnectingVmix = false;
                this->sendShowFatalErrorScreen.fire("Unable to connect to vMix!");
                return;
            }
            Serial.print(".");
            M5.Lcd.print(".");
            this->_pollReconnectVmixTimer.setNextExecute(timestamp + WAIT_INTERVAL_MS);
            return;
        }
        Serial.println();
        M5.Lcd.println();

        this->_context->setIsVmixConnected(true);
        this->_isConnectingVmix = false;

        Serial.println("Connection opened.");
        Serial.println("Subscribing to tally events...");
        this->_vmixMgr->sendSubscribeTally();
        this->show();
    }
};

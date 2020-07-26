#ifndef CONNSCREEN_H
#define CONNSCREEN_H

#define SCREEN_CONN 2

#define NUM_WAITS 10
#define WAIT_INTERVAL_MS 1000

#define TALLY_NONE '?'
#define TALLY_SAFE '0'
#define TALLY_LIVE '1'
#define TALLY_PRE '2'

#define ESP32

#include "Screen.h"

#include <M5StickC.h>
#include <PinButton.h>
#include "AppState.h"
#include "WifiManager.h"

// TODO split into wifi connection screen and vmix connection screen

class ConnectingScreen : public Screen
{
public:
    ConnectingScreen(AppState &appState) : Screen(appState), _wifiMgr(appState) {}
    ~ConnectingScreen();

    unsigned int Screen::getId() { return SCREEN_CONN; }

    void show()
    {
        if (!reconnectWifi())
        {
            // main_showErrorScreen("Could not connect to wifi!"); // TODO fire show error screen event that ScreenManager will listen for
            this->_appState->setIsVmixConnected(false);
            return;
        }

        if (!reconnectVmix())
        {
            main_showErrorScreen("Could not connect to vMix!");
            return;
        }

        vmix_showTallyScreen(settings.getVmixTally());
    }

    void refresh()
    {
    }

    void handleInput(unsigned long timestamp, PinButton m5Btn, PinButton sideBtn)
    {
    }

    bool reconnectWifi()
    {
        this->_appState->setIsWifiConnected(false);

        auto settings = this->_appState->getSettings();

        const char *ssid = settings->getWifiSsid();
        const char *passphrase = settings->getWifiPassphrase();

        Serial.printf("SSID: %s\n", ssid);
        Serial.printf("Pass: %s\n", passphrase);
        Serial.printf("Connecting to %s with %s...", ssid, passphrase);

        M5.Lcd.fillScreen(TFT_BLACK);
        M5.Lcd.setCursor(0, 0);
        // main_updateOrientation(0); // TODO fire orientation change event that ScreenManager will listen for
        M5.Lcd.setTextSize(1);
        // main_setScreenColors(WHITE, BLACK); // TODO color change event that ScreenManager will listen for
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
            this->_appState->setIsWifiConnected(true);

            Serial.println("Connected!");
            M5.Lcd.println("Connected!");
            Serial.print("IP: ");
            Serial.println(this->_wifiMgr->localIP());
            return true;
        }

        Serial.printf("Unable to connect to %s!\n", ssid);
        // main_updateOrientation(0); // TODO fire orientation change event that ScreenManager will listen for
        M5.Lcd.printf("Unable to connect to %s!\n", ssid);

        return false;
    }

    // TODO refactor into wifi class
    bool reconnectVmix()
    {
        this->_appState->setIsVmixConnected(false);
        this->_appState->setTallyState(TALLY_NONE);
        // vmix_renderTallyScreen(settings.getVmixTally()); // TODO fire screen change event that screen manager catches

        auto settings = this->_appState->getSettings();

        if (!this->_appState->getIsWifiConnected())
        {
            if (!reconnectWifi())
            {
                return false;
            }
        }

        if (!vmix_connect(settings->getVmixAddress(), settings->getVmixPort()))
        {
            // main_updateOrientation(0); // TODO fire orientation change event that screen manager catches
            Serial.println("Unable to connect to vMix!");
            M5.Lcd.println("Unable to connect to vMix!");
            return false;
        }

        this->_appState->setIsVmixConnected(true);
        return true;
    }

    // TODO refactor into VmixClient
    bool vmix_connect(const char *addr, unsigned short port)
    {
        M5.Lcd.fillScreen(TFT_BLACK);
        M5.Lcd.setCursor(0, 0);
        // main_updateOrientation(0); // TODO fire orientation change event that screen manager catches
        M5.Lcd.setTextSize(1);
        // main_setScreenColors(WHITE, BLACK); // TODO fire color change event that screen manager catches
        Serial.println("Connecting to vMix...");
        M5.Lcd.println("Connecting to vMix...");

        if (vmix_client.connected())
        {
            vmix_client.stop();
        }

        byte retryAttempt = 0;
        byte maxRetry = VMIX_CONN_RETRIES;
        do
        {
            if (!vmix_client.connect(addr, port))
            {
                if (retryAttempt == maxRetry)
                {
                    return false;
                }

                retryAttempt++;
                Serial.printf("Retrying %d/%d...\n", retryAttempt, maxRetry);
                M5.Lcd.printf("Retrying %d/%d...\n", retryAttempt, maxRetry);
                delay(1000);

                continue;
            }

            Serial.println("Connection opened.");
            Serial.println("Subscribing to tally events...");
            vmix_client.print(VMIX_API_SUBSCRIBE_TALLY);

            return true;
        } while (true);

        return false;
    }

private:
    WifiManager *_wifiMgr;
};

#endif
// hardware
#define ESP32
#define LOG_BATTERY 1
#include <M5StickC.h>

// libraries
#include <PinButton.h>

// app
#include "AppContext.h"
#include "Screen.h"

class SettingsScreen : public Screen
{
public:
    SettingsScreen(AppContext &context) : Screen(context) {}
    ~SettingsScreen();

    unsigned int getId() { return SCREEN_SETTINGS; }

    void show()
    {
        this->refresh();
    }

    void refresh()
    {
        auto settingsIdx = this->_context->getSettingsIdx();
        auto numSettings = this->_context->getNumSettings();
        auto settings = this->_context->getSettings();
        auto settingsMgr = this->_context->getSettingsManager();

        this->sendOrientationChange.fire(LANDSCAPE);
        this->sendColorChange.fire(Colors(TFT_WHITE, TFT_BLACK));
        M5.Lcd.fillScreen(TFT_BLACK);
        M5.Lcd.setTextDatum(MC_DATUM);

        M5.Lcd.printf("SETTINGS: %d/%d\n", settingsIdx + 1, numSettings);
        M5.Lcd.printf("-SSID: %s\n", settings->getWifiSsid());
        M5.Lcd.printf("-IP: ");
        M5.Lcd.println(this->_context->getWifiManager()->localIP());
        M5.Lcd.printf("-vMix: %s\n", settings->getVmixAddressWithPort());
        M5.Lcd.printf("-TALLY: %d\n", settings->getVmixTally());

        unsigned long timestamp = millis();
        unsigned long hours = timestamp / 1000 / 60 / 60;
        unsigned long minutes = (timestamp - (hours * 1000 * 60 * 60)) / 1000 / 60;
        unsigned long seconds = (timestamp - (hours * 1000 * 60 * 60) - (minutes * 1000 * 60)) / 1000;
        M5.Lcd.printf("Runtime: %02u:%02u:%02u\n", hours, minutes, seconds);

        if (this->_context->getIsCharging())
        {
            M5.Lcd.printf("Battery: CHARGING\n");
        }
        else
        {
            M5.Lcd.printf("Battery: %3.0f%%\n", this->_context->getBatteryLevel());
        }
        timestamp = settingsMgr->getLastUptime();
        double lastBatteryLevel = settingsMgr->getLastBatteryLevel();

        if (LOG_BATTERY)
        {
            hours = timestamp / 1000 / 60 / 60;
            minutes = (timestamp - (hours * 1000 * 60 * 60)) / 1000 / 60;
            seconds = (timestamp - (hours * 1000 * 60 * 60) - (minutes * 1000 * 60)) / 1000;
            M5.Lcd.printf("LAST RUN: %02u:%02u:%02u up, %.0f%% batt\n", hours, minutes, seconds, lastBatteryLevel);
        }

        M5.Lcd.println();
        M5.Lcd.println("Hold side btn to swap settings.");
    }

    void handleInput(unsigned long timestamp, PinButton &m5Btn, PinButton &sideBtn)
    {
        if (m5Btn.isSingleClick())
        {
            this->sendScreenChange.fire(SCREEN_TALLY);
        }
        if (sideBtn.isLongClick())
        {
            Serial.println("Swapping settings...");
            this->_context->cycleSettings();
            Serial.println("Settings swapped!");
            this->sendScreenChange.fire(SCREEN_CONN);
            return;
        }
    }
};

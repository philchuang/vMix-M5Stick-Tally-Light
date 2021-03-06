// hardware
#define ESP32
#include <M5StickC.h>

// libraries
#include <PinButton.h>

// app
#include "AppContext.h"
#include "Screen.h"

class ErrorScreen : public Screen
{
public:
    ErrorScreen(AppContext &context) : Screen(this, context) {}
    ~ErrorScreen() {}

    void setMessage(const char *message)
    {
        this->_message = message;
    }

    unsigned int getId() { return SCREEN_ERROR; }

    void show()
    {
        M5.Lcd.fillScreen(TFT_BLACK);

        this->sendOrientationChange.fire(LANDSCAPE);
        this->sendColorChange.fire(Colors(TFT_WHITE, TFT_BLACK));

        // display errormessage
        M5.Lcd.setCursor(0, 0);
        M5.Lcd.setTextSize(1);
        M5.Lcd.println(this->_message);

        M5.Lcd.println();

        // display settings
        auto settings = this->_context->getSettings();
        auto settingsIdx = this->_context->getSettingsIdx();
        auto numSettings = this->_context->getNumSettings();
        auto ssid = settings->getWifiSsid();
        auto ip = this->_context->getWifiManager()->localIP();
        auto vmix = settings->getVmixAddressWithPort();
        M5.Lcd.printf("SETTINGS: %d/%d\n", settingsIdx + 1, numSettings);
        M5.Lcd.printf("-SSID: %s\n", ssid);
        M5.Lcd.printf("-IP: ");
        M5.Lcd.println(ip);
        M5.Lcd.printf("-vMix: %s\n", vmix);

        M5.Lcd.println();

        // display instructions
        M5.Lcd.println("Press M5 button to retry.");
        M5.Lcd.println("Press side button to switch settings.");
    }

    void refresh()
    {
        // does nothing
    }

    void handleInput(unsigned long timestamp, PinButton &m5Btn, PinButton &sideBtn)
    {
        // m5 button single click to retry
        if (m5Btn.isSingleClick())
        {
            this->_message = 0;
            this->sendScreenChange.fire(SCREEN_CONN);
            return;
        }

        // side button single click to switch settings
        else if (sideBtn.isSingleClick())
        {
            this->_message = 0;
            this->_context->cycleSettings();
            this->sendScreenChange.fire(SCREEN_CONN);
            return;
        }
    }

private:
    const char *_message;
};
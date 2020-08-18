// hardware
#define ESP32
#include <M5StickC.h>

// libraries
#include <PinButton.h>

// app
#include "AppContext.h"
#include "Screen.h"

class SplashScreen : public Screen
{
public:
    SplashScreen(AppContext &context) : Screen(this, context) {}
    ~SplashScreen() { }

    unsigned int getId() { return SCREEN_SPLASH; }

    void show()
    {
        this->sendOrientationChange.fire(LANDSCAPE);
        this->sendColorChange.fire(Colors(TFT_WHITE, TFT_BLACK));
        M5.Lcd.fillScreen(TFT_BLACK);
        M5.Lcd.setTextDatum(MC_DATUM);
        M5.Lcd.drawString("vMix M5Stick-C Tally", 80, 15, 2);
        M5.Lcd.drawString("by Phil Chuang", 80, 35, 1);
        M5.Lcd.drawString("& Guido Visser", 80, 55, 1);

        delay(2000);
        this->_context->loadSettings(0);
        this->sendScreenChange.fire(SCREEN_CONN);
    }

    void refresh()
    {
        // does nothing
    }

    void handleInput(unsigned long timestamp, PinButton &m5Btn, PinButton &sideBtn)
    {
        // does nothing
    }
};

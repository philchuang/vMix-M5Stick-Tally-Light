// hardware
#define ESP32
#include <M5StickC.h>

// libraries
#include <PinButton.h>
#include <Callback.h>

// app
#include "AppContext.h"
#include "SlotLoopEvent.h"
#include "Screen.h"

#define SPLASH_WAIT_MS 2000u

class SplashScreen : public Screen
{
public:
    SplashScreen(AppContext &context) : Screen(this, context),
                                        _changeScreenDelegate(this, &SplashScreen::changeScreen),
                                        _changeScreenTimer(_changeScreenDelegate, -1)
    {
    }
    ~SplashScreen()
    {
    }

    unsigned int getId() { return SCREEN_SPLASH; }

    void show()
    {
        this->sendOrientationChange.fire(LANDSCAPE);
        this->sendColorChange.fire(Colors(TFT_WHITE, TFT_BLACK));

        this->refresh();

        this->_changeScreenTimer.setNextExecute(millis() + SPLASH_WAIT_MS);
        this->_context->loadSettings(0);
    }

    void refresh()
    {
        M5.Lcd.fillScreen(TFT_BLACK);
        M5.Lcd.setTextDatum(MC_DATUM);
        M5.Lcd.drawString("vMix M5Stick-C Tally", 80, 15, 2);
        M5.Lcd.drawString("by Phil Chuang", 80, 35, 1);
        M5.Lcd.drawString("& Guido Visser", 80, 55, 1);
    }

    void handleInput(unsigned long timestamp, PinButton &m5Btn, PinButton &sideBtn)
    {
        this->_changeScreenTimer.execute(timestamp);
    }

private:
    MethodSlot<SplashScreen, unsigned long> _changeScreenDelegate;
    SlotLoopEvent _changeScreenTimer;

    void changeScreen(unsigned long unused)
    {
        this->sendScreenChange.fire(SCREEN_CONN);
    }
};

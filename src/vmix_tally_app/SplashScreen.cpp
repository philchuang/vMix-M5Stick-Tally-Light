#define SCREEN_SPLASH 1

#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#define ESP32

#include "Screen.h"

#include <M5StickC.h>
#include <PinButton.h>
#include "OrientationManager.h"
#include "AppState.h"

class SplashScreen : public Screen
{
public:
    SplashScreen(AppState &state) : Screen(state) {}
    ~SplashScreen();

    unsigned int getId() { return SCREEN_SPLASH; }

    void show()
    {
        this->refresh();
        delay(2000);
    }

    void refresh()
    {
        if (this->orientationChangeHandler) this->orientationChangeHandler(LANDSCAPE);
        if (this->colorChangeHandler) this->colorChangeHandler(TFT_WHITE, TFT_BLACK);
        M5.Lcd.fillScreen(TFT_BLACK);
        M5.Lcd.setTextDatum(MC_DATUM);
        M5.Lcd.drawString("vMix M5Stick-C Tally", 80, 15, 2);
        M5.Lcd.drawString("by Phil Chuang", 80, 35, 1);
        M5.Lcd.drawString("& Guido Visser", 80, 55, 1);
    }

    void handleInput(unsigned long timestamp, PinButton m5Btn, PinButton sideBtn)
    {
        // does nothing
    }
};


#endif
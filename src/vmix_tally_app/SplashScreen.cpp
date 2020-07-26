#ifndef CONNSPLASH_H
#define CONNSPLASH_H

#define ESP32

#define SCREEN_SPLASH 1

#include <M5StickC.h>
#include <PinButton.h>
#include "AppState.h"
#include "Screen.h"

class SplashScreen : public Screen
{
public:
    SplashScreen() : Screen() {}
    ~SplashScreen();

    unsigned int Screen::getId() { return SCREEN_SPLASH; }

    void show()
    {
        this->refresh();
        delay(2000);
    }

    void refresh()
    {
        // main_updateOrientation(0); // TODO fire orientation change event that ScreenManager will listen for
        M5.Lcd.fillScreen(TFT_BLACK);
        M5.Lcd.setTextDatum(MC_DATUM);
        M5.Lcd.drawString("vMix M5Stick-C Tally", 80, 15, 2);
        M5.Lcd.drawString("by Phil Chuang", 80, 35, 1);
        M5.Lcd.drawString("& Guido Visser", 80, 55, 1);
    }

    void handleInput(unsigned long timestamp, PinButton m5Btn, PinButton sideBtn)
    {
    }
};

#endif
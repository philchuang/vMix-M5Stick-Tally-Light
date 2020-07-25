#ifndef CONNSCREEN_H
#define CONNSCREEN_H

#include <PinButton.h>
#include "AppState.h"
#include "Screen.h"

#define SCREEN_CONN 101

class ConnectingScreen : public Screen
{
public:
    ConnectingScreen(AppState appState) : Screen(SCREEN_CONN, appState) {}
    ~ConnectingScreen();

    void show()
    {
    }

    void refresh()
    {
    }

    void handleInput(unsigned long timestamp, PinButton m5Btn, PinButton sideBtn)
    {
    }
};

#endif
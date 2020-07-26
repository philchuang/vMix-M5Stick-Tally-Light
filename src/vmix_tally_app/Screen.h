#ifndef SCREEN_H
#define SCREEN_H

#include <PinButton.h>
#include "AppState.h"

class Screen
{
public:
    Screen();
    ~Screen();

    void setAppState(AppState state)
    {
        this->_appState = state;
    }
    virtual unsigned int getId() = 0;
    virtual void show() = 0;
    virtual void refresh() = 0;
    virtual void handleInput(unsigned long timestamp, PinButton m5Btn, PinButton sideBtn) = 0;

private:
    AppState _appState;
};

#endif
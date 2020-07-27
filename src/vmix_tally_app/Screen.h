#ifndef SCREEN_H
#define SCREEN_H

#include <PinButton.h>
#include "AppState.h"

class Screen
{
public:
    Screen(AppState &state) : _appState(state);
    ~Screen();

    virtual unsigned int getId() = 0;
    virtual void show() = 0;
    virtual void refresh() = 0;
    virtual void handleInput(unsigned long timestamp, PinButton m5Btn, PinButton sideBtn) = 0;

protected:
    AppState *_appState;
    ScreenManager *_mgr;
};

#endif
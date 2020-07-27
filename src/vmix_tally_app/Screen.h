#ifndef SCREEN_H
#define SCREEN_H

#include <PinButton.h>
#include "AppState.h"

typedef void (*OrientationChangeHandler)(unsigned short orientation);
typedef void (*ColorChangeHandler)(unsigned short foreColor, unsigned short backColor);
typedef void (*ScreenChangeHandler)(unsigned short screenId);
typedef void (*ShowErrorScreenHandler)(const char *message);

class Screen
{
public:
    Screen(AppState &state) : _appState(&state) {}
    ~Screen();

    OrientationChangeHandler orientationChangeHandler = 0;
    ColorChangeHandler colorChangeHandler = 0;
    ScreenChangeHandler screenChangeHandler = 0;
    ShowErrorScreenHandler showFatalErrorScreenHandler = 0;

    virtual unsigned int getId() = 0;
    virtual void show() = 0;
    virtual void refresh() = 0;
    virtual void handleInput(unsigned long timestamp, PinButton m5Btn, PinButton sideBtn) = 0;

protected:
    AppState *_appState;
};

#endif
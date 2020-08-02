#define SCREEN_SPLASH 1
#define SCREEN_CONN   2
#define SCREEN_TALLY  3

typedef void (*OrientationChangeHandler)(unsigned short orientation);
typedef void (*ColorChangeHandler)(unsigned short foreColor, unsigned short backColor);
typedef void (*ScreenChangeHandler)(unsigned short screenId);
typedef void (*ShowErrorScreenHandler)(const char *message);

#ifndef SCREEN_H
#define SCREEN_H

#include <PinButton.h>
#include "AppContext.h"

class Screen
{
public:
    Screen(AppContext &context) : _context(&context) {}
    ~Screen();

    OrientationChangeHandler orientationChangeHandler = 0;
    ColorChangeHandler colorChangeHandler = 0;
    ScreenChangeHandler screenChangeHandler = 0;
    ShowErrorScreenHandler showFatalErrorScreenHandler = 0;

    void unregister()
    {
        this->orientationChangeHandler = 0;
        this->colorChangeHandler = 0;
        this->screenChangeHandler = 0;
        this->showFatalErrorScreenHandler = 0;
    }

    virtual unsigned int getId() = 0;
    virtual void show() = 0;
    virtual void refresh() = 0;
    virtual void handleInput(unsigned long timestamp, PinButton m5Btn, PinButton sideBtn) = 0;

protected:
    AppContext *_context;
};

#endif
#define SCREEN_ERROR    0
#define SCREEN_SPLASH   1
#define SCREEN_CONN     2
#define SCREEN_TALLY    3
#define SCREEN_SETTINGS 4

#define LANDSCAPE 0
#define PORTRAIT  1

#ifndef SCREEN_H
#define SCREEN_H

#include <PinButton.h>
#include <Callback.h>
#include "Colors.cpp"
#include "AppContext.h"

class Screen
{
public:
    Screen(AppContext &context) : _context(&context) {}
    ~Screen();

    Signal<unsigned short> orientationChangeHandler;
    Signal<unsigned long> cycleBacklightHandler;
    Signal<Colors> colorChangeHandler;
    Signal<unsigned short> screenChangeHandler;
    Signal<const char *> showFatalErrorScreenHandler;

    void unregister()
    {
        this->orientationChangeHandler.~Signal();
        this->cycleBacklightHandler.~Signal();
        this->colorChangeHandler.~Signal();
        this->screenChangeHandler.~Signal();
        this->showFatalErrorScreenHandler.~Signal();
    }

    virtual unsigned int getId() = 0;
    virtual void show() = 0;
    virtual void refresh() = 0;
    virtual void handleInput(unsigned long timestamp, PinButton &m5Btn, PinButton &sideBtn) = 0;

protected:
    AppContext *_context;
};

#endif
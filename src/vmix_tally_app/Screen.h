#define SCREEN_ERROR    0u
#define SCREEN_SPLASH   1u
#define SCREEN_CONN     2u
#define SCREEN_TALLY    3u
#define SCREEN_SETTINGS 4u

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

    Signal<unsigned short> sendOrientationChange;
    Signal<unsigned long> sendCycleBacklight;
    Signal<Colors> sendColorChange;
    Signal<unsigned short> sendScreenChange;
    Signal<const char *> sendShowFatalErrorScreen;

    void unregister()
    {
        this->sendOrientationChange.~Signal();
        this->sendCycleBacklight.~Signal();
        this->sendColorChange.~Signal();
        this->sendScreenChange.~Signal();
        this->sendShowFatalErrorScreen.~Signal();
    }

    virtual void onScreenChanged(unsigned short screenId)
    {
        // default implementation does nothing
    }

    virtual unsigned int getId() = 0;
    virtual void show() = 0;
    virtual void refresh() = 0;
    virtual void handleInput(unsigned long timestamp, PinButton &m5Btn, PinButton &sideBtn) = 0;

protected:
    AppContext *_context;
};

#endif
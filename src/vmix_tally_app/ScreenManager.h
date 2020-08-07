/* ScreenManager.h - class that handles screen rendering
 */

#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

// libraries
#include <Callback.h>
#include <PinButton.h>

// app
#include "AppContext.h"
#include "Screen.h"

class ScreenManager
{
public:
    ScreenManager(AppContext &context, unsigned int maxScreens);
    ~ScreenManager();

    Signal<unsigned short> sendOrientationChange;

    void begin();
    void add(Screen *screen);
    Screen *getCurrent();
    void show(unsigned short screenId);
    void refresh();
    void handleInput(unsigned long timestamp, PinButton &m5Btn, PinButton &sideBtn);

protected:
    void onOrientationChange(unsigned short orientation);
    void onCycleBacklight(unsigned long timestamp);
    void onColorChange(Colors colors);
    void onShowFatalErrorScreen(const char *message);
    void pollForceRefresh(unsigned long timestamp);

private:
    class Impl;
    Impl *_pimpl;
};

#endif

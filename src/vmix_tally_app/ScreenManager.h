/* ScreenManager.h - class that handles screen rendering
 */

#ifndef ScreenManager_h
#define ScreenManager_h

#include <PinButton.h>
#include "AppState.h"
#include "Screen.h"

class ScreenManager
{
public:
    ScreenManager(AppState &state, unsigned int maxScreens);
    ~ScreenManager();

    void begin();
    void add(Screen &screen);
    Screen* getCurrent();
    void show(unsigned int screen);
    void refresh();
    void handleInput(unsigned long timestamp, PinButton m5Btn, PinButton sideBtn);

private:
    class Impl;
    Impl *_pimpl;
};

#endif

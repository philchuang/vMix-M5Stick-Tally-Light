/* ScreenManager.h - class that handles screen rendering
 */

#ifndef ScreenManager_h
#define ScreenManager_h

#include "AppState.h"

class ScreenManager
{
public:
    ScreenManager(AppState state);
    ~ScreenManager();

    void begin();
    
    unsigned int getCurrentScreen();
    void setCurrentScreen(unsigned int screen);

private:
    class Impl;
    Impl *_pimpl;
};

#endif

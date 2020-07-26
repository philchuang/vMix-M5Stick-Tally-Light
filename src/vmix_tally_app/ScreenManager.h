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
    void add(Screen* screen);
    unsigned int getCurrent();
    void show(unsigned int screen);

private:
    class Impl;
    Impl *_pimpl;
};

#endif

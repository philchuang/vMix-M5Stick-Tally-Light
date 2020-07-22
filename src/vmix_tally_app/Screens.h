/* Screens.h - class that handles screen rendering
 */

#ifndef Screens_h
#define Screens_h

#include "AppState.h"

class Screens
{
public:
    Screens(AppState state);
    ~Screens();

    void begin();
    
    unsigned int getCurrentScreen();
    void setCurrentScreen(unsigned int screen);

private:
    class Impl;
    Impl *_pimpl;
};

#endif

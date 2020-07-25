#ifndef SCREEN_H
#define SCREEN_H

#include <PinButton.h>
#include "AppState.h"

class Screen
{
public:
    Screen(unsigned int id, AppState appState) : _id(id), _appState(appState) {}
    ~Screen();

    unsigned int getId() { return this->_id; }
    virtual void show() = 0;
    virtual void refresh() = 0;
    virtual void handleInput(unsigned long timestamp, PinButton m5Btn, PinButton sideBtn) = 0;

private:
    unsigned int _id;
    AppState _appState;
};

#endif
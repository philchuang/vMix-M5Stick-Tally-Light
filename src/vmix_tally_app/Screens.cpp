#define ESP32

#include "Screens.h"

#include "AppState.h"
#include <stdio.h>
#include <string.h>
#include <M5StickC.h>

struct Screens::Impl
{
    Impl(AppState state) : _state(state)
    {
    }

    ~Impl()
    {
    }

    AppState _state;
    unsigned int _currentScreen;
};

Screens::Screens(AppState state)
    : _pimpl(new Impl(state))
{
}

Screens::~Screens() = default;

void Screens::begin()
{
    // TODO implement
}

unsigned int Screens::getCurrentScreen()
{
    return _pimpl->_currentScreen;
}

void Screens::setCurrentScreen(unsigned int screen)
{
    _pimpl->_currentScreen = screen;
    // TODO render screen
}
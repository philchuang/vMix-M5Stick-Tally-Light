#define ESP32

#include "ScreenManager.h"

#include "AppState.h"
#include <stdio.h>
#include <string.h>
#include <M5StickC.h>

struct ScreenManager::Impl
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

ScreenManager::ScreenManager(AppState state)
    : _pimpl(new Impl(state))
{
}

ScreenManager::~ScreenManager() = default;

void ScreenManager::begin()
{
    // TODO implement
}

unsigned int ScreenManager::getCurrentScreen()
{
    return _pimpl->_currentScreen;
}

void ScreenManager::setCurrentScreen(unsigned int screen)
{
    _pimpl->_currentScreen = screen;
    // TODO render screen
}
#define ESP32

#include "ScreenManager.h"

#include "AppState.h"
#include <stdio.h>
#include <map>
#include <string.h>
#include <M5StickC.h>
#include "Screen.h"

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
    std::map<unsigned int, std::unique_ptr<Screen>> _screenMap;
};

ScreenManager::ScreenManager(AppState state)
    : _pimpl(new Impl(state))
{
}

ScreenManager::~ScreenManager() = default;

void ScreenManager::begin()
{
    // TODO anything go here?
}

void ScreenManager::add(Screen* screen)
{
    screen->setAppState(_pimpl->_state);
    _pimpl->_screenMap[screen->getId()] = std::unique_ptr<Screen> (screen);
}

unsigned int ScreenManager::getCurrent()
{
    return _pimpl->_currentScreen;
}

void ScreenManager::show(unsigned int screen)
{
    _pimpl->_currentScreen = screen;
    // TODO render screen
}
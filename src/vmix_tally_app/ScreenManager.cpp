#define ESP32

#include "ScreenManager.h"

#include <stdio.h>
#include <vector>
#include "AppContext.h"
#include "Screen.h"

struct ScreenManager::Impl
{
    Impl(AppContext &context, unsigned int maxScreens) : _context(&context), _screens(maxScreens)
    {
    }

    ~Impl()
    {
    }

    AppContext *_context;
    unsigned int _currentScreen;
    std::vector<Screen *> _screens;
};

ScreenManager::ScreenManager(AppContext &context, unsigned int maxScreens)
    : _pimpl(new Impl(context, maxScreens))
{
}

ScreenManager::~ScreenManager()
{
    _pimpl->_context = 0;
    for (auto it = _pimpl->_screens.begin(); it != _pimpl->_screens.end(); ++it)
    {
        (*it)->unregister();
    }
    _pimpl->_screens.clear();
}

void ScreenManager::begin()
{
    // TODO anything go here?
}

void ScreenManager::add(Screen &screen)
{
    // TODO register event handlers
    screen.orientationChangeHandler = this->orientationChangeHandler;
    _pimpl->_screens[screen.getId()] = &screen;
}

Screen *ScreenManager::getCurrent()
{
    return _pimpl->_screens[_pimpl->_currentScreen];
}

void ScreenManager::show(unsigned int screenId)
{
    _pimpl->_currentScreen = screenId;
    this->refresh();
}

void ScreenManager::refresh()
{
    auto screen = this->getCurrent();
    screen->refresh();
}

void ScreenManager::handleInput(unsigned long timestamp, PinButton m5Btn, PinButton sideBtn)
{
    auto screen = this->getCurrent();
    screen->handleInput(timestamp, m5Btn, sideBtn);
}
#define ESP32

#include "ScreenManager.h"

#include <M5StickC.h>
#include <stdio.h>
#include <vector>
#include "AppContext.h"
#include "Screen.h"
#include "ErrorScreen.cpp"

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

    unsigned short _lastForegroundColor;
    unsigned short _lastBackgroundColor;
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

    this->orientationChangeHandler.~Signal();
}

void ScreenManager::begin()
{
    // TODO anything go here?
}

void ScreenManager::add(Screen &screen)
{
    MethodSlot<ScreenManager, unsigned short> orientationChangeListener(this, &ScreenManager::onOrientationChange);
    screen.orientationChangeHandler.attach(orientationChangeListener);
    MethodSlot<ScreenManager, unsigned long> cycleBacklightListener(this, &ScreenManager::onCycleBacklight);
    screen.cycleBacklightHandler.attach(cycleBacklightListener);
    MethodSlot<ScreenManager, Colors> colorChangeListener(this, &ScreenManager::onColorChange);
    screen.colorChangeHandler.attach(colorChangeListener);
    MethodSlot<ScreenManager, unsigned short> screenChangeListener(this, &ScreenManager::show);
    screen.screenChangeHandler.attach(screenChangeListener);
    MethodSlot<ScreenManager, const char *> showFatalErrorScreenListener(this, &ScreenManager::showFatalErrorScreen);
    screen.showFatalErrorScreenHandler.attach(showFatalErrorScreenListener);
    
    _pimpl->_screens[screen.getId()] = &screen;
}

void ScreenManager::onOrientationChange(unsigned short orientation)
{
    this->orientationChangeHandler.fire(orientation);
}

void ScreenManager::onCycleBacklight(unsigned long timestamp)
{
    this->cycleBacklightHandler.fire(timestamp);
}

void ScreenManager::onColorChange(Colors colors)
{
    _pimpl->_lastForegroundColor = colors.foreColor;
    _pimpl->_lastBackgroundColor = colors.backColor;
    M5.Lcd.setTextColor(colors.foreColor, colors.backColor);
}

void ScreenManager::showFatalErrorScreen(const char *message)
{
    auto errorScreen = reinterpret_cast<ErrorScreen *>(_pimpl->_screens[SCREEN_ERROR]);
    errorScreen->setMessage(message);
    this->show(SCREEN_ERROR);
}

Screen *ScreenManager::getCurrent()
{
    return _pimpl->_screens[_pimpl->_currentScreen];
}

void ScreenManager::show(unsigned short screenId)
{
    _pimpl->_currentScreen = screenId;
    this->refresh();
}

void ScreenManager::refresh()
{
    auto screen = this->getCurrent();
    screen->refresh();
}

void ScreenManager::handleInput(unsigned long timestamp, PinButton &m5Btn, PinButton &sideBtn)
{
    auto screen = this->getCurrent();
    screen->handleInput(timestamp, m5Btn, sideBtn);
}
#define ESP32

#include "ScreenManager.h"

#include <M5StickC.h>
#include <stdio.h>
#include <vector>
#include <Callback.h>
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
    Signal<unsigned short> _sendScreenChanged;
};

ScreenManager::ScreenManager(AppContext &context, unsigned int maxScreens)
    : _pimpl(new Impl(context, maxScreens))
{
}

ScreenManager::~ScreenManager()
{
    _pimpl->_sendScreenChanged.~Signal();
    _pimpl->_context = 0;
    for (auto it = _pimpl->_screens.begin(); it != _pimpl->_screens.end(); ++it)
    {
        (*it)->unregister();
    }
    _pimpl->_screens.clear();

    this->sendOrientationChange.~Signal();
}

void ScreenManager::begin()
{
    // TODO anything go here?
}

void ScreenManager::add(Screen &screen)
{
    MethodSlot<ScreenManager, unsigned short> orientationChangeListener(this, &ScreenManager::onOrientationChange);
    screen.sendOrientationChange.attach(orientationChangeListener);
    MethodSlot<ScreenManager, unsigned long> cycleBacklightListener(this, &ScreenManager::onCycleBacklight);
    screen.sendCycleBacklight.attach(cycleBacklightListener);
    MethodSlot<ScreenManager, Colors> colorChangeListener(this, &ScreenManager::onColorChange);
    screen.sendColorChange.attach(colorChangeListener);
    MethodSlot<ScreenManager, unsigned short> screenChangeListener(this, &ScreenManager::show);
    screen.sendScreenChange.attach(screenChangeListener);
    MethodSlot<ScreenManager, const char *> showFatalErrorScreenListener(this, &ScreenManager::onShowFatalErrorScreen);
    screen.sendShowFatalErrorScreen.attach(showFatalErrorScreenListener);
    MethodSlot<Screen, unsigned short> onScreenChangedListener(&screen, &Screen::onScreenChanged);
    _pimpl->_sendScreenChanged.attach(onScreenChangedListener);
    _pimpl->_screens[screen.getId()] = &screen;
}

void ScreenManager::onOrientationChange(unsigned short orientation)
{
    this->sendOrientationChange.fire(orientation);
}

void ScreenManager::onCycleBacklight(unsigned long timestamp)
{
    // TODO replace this with main loop event and integrate this class into the main loop
    this->sendCycleBacklight.fire(timestamp);
}

void ScreenManager::onColorChange(Colors colors)
{
    _pimpl->_lastForegroundColor = colors.foreColor;
    _pimpl->_lastBackgroundColor = colors.backColor;
    M5.Lcd.setTextColor(colors.foreColor, colors.backColor);
}

void ScreenManager::onShowFatalErrorScreen(const char *message)
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
    _pimpl->_sendScreenChanged.fire(screenId);
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
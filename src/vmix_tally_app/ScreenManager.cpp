#define ESP32

#define APP_SCREENREFRESH_MS 10000u
#define APP_BRIGHTNESS_TIMEOUT_MS 1000u

#include "ScreenManager.h"

#include <M5StickC.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <Callback.h>
#include "AppContext.h"
#include "Screen.h"
#include "ErrorScreen.cpp"
#include "LoopEvent.h"

struct ScreenManager::Impl
{
    Impl(AppContext &context, unsigned int maxScreens) : _context(&context), _screens(maxScreens)
    {
    }

    ~Impl()
    {
    }

    Signal<unsigned short> _sendScreenChanged;

    AppContext *_context;
    unsigned int _currentScreen;
    std::vector<Screen *> _screens;
    SlotLoopEvent *_screenRefreshCheck;

    unsigned short _lastForegroundColor;
    unsigned short _lastBackgroundColor;
    unsigned int _lastBrightness = 0;
};

ScreenManager::ScreenManager(AppContext &context, unsigned int maxScreens)
    : _pimpl(new Impl(context, maxScreens))
{
    MethodSlot<ScreenManager, unsigned long> forceRefreshListener(this, &ScreenManager::pollForceRefresh);
    auto temp = SlotLoopEvent(forceRefreshListener, APP_SCREENREFRESH_MS);
    _pimpl->_screenRefreshCheck = &temp;
}

ScreenManager::~ScreenManager()
{
    this->sendOrientationChange.~Signal();
    _pimpl->_sendScreenChanged.~Signal();
    delete _pimpl->_screenRefreshCheck;
    
    for (auto it = _pimpl->_screens.begin(); it != _pimpl->_screens.end(); ++it)
    {
        (*it)->unregister();
    }
    _pimpl->_screens.clear();

    delete _pimpl->_context;
}

void ScreenManager::begin()
{
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
    _pimpl->_screenRefreshCheck->setNextExecute(timestamp + APP_BRIGHTNESS_TIMEOUT_MS);

    char *brightnessString = new char[4];
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextDatum(MC_DATUM);

    // blit out previous message
    M5.Lcd.setTextColor(_pimpl->_lastBackgroundColor);
    sprintf(brightnessString, "%d%%", _pimpl->_lastBrightness);
    
    auto orientation = _pimpl->_context->getOrientation();
    if (orientation == LANDSCAPE)
    {
        M5.Lcd.drawString(brightnessString, 80, 80, 1);
    }
    else
    {
        M5.Lcd.drawString(brightnessString, 40, 160, 1);
    }

    _pimpl->_lastBrightness = _pimpl->_context->cycleBacklight();

    // display new message
    M5.Lcd.setTextColor(_pimpl->_lastForegroundColor);
    sprintf(brightnessString, "%d%%", _pimpl->_lastBrightness);
    if (orientation == LANDSCAPE)
    {
        M5.Lcd.drawString(brightnessString, 80, 80, 1);
    }
    else
    {
        M5.Lcd.drawString(brightnessString, 40, 160, 1);
    }
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
    _pimpl->_screenRefreshCheck->execute(timestamp);
    auto screen = this->getCurrent();
    screen->handleInput(timestamp, m5Btn, sideBtn);
}

void ScreenManager::pollForceRefresh(unsigned long timestamp)
{
    this->refresh();
}
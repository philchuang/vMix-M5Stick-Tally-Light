#include "ScreenManager.h"

// hardware
#define ESP32
#include <Arduino.h>
#include <M5StickC.h>

// libraries
#include <stdio.h>
#include <string>
#include <vector>
#include <Callback.h>

// app
#include "AppContext.h"
#include "Constants.h"
#include "Configuration.h"
#include "OrientationManager.h"
#include "Screen.h"
#include "SlotLoopEvent.h"

// screens
#include "ConnectingScreen.cpp"
#include "ErrorScreen.cpp"
#include "SettingsScreen.cpp"
#include "SplashScreen.cpp"
#include "TallyScreen.cpp"

// constants
#define APP_SCREENREFRESH_MS 10000u
#define APP_BRIGHTNESS_TIMEOUT_MS 1000u

struct ScreenManager::Impl
{
    Impl(ScreenManager *self, AppContext &context, unsigned int maxScreens) : _context(&context),
                                                                              _screens(maxScreens),
                                                                              _pollForceRefreshDelegate(self, &ScreenManager::pollForceRefresh),
                                                                              _pollForceRefreshTimer(_pollForceRefreshDelegate, APP_SCREENREFRESH_MS),
                                                                              _pollOrientationCheckDelegate(self, &ScreenManager::pollOrientationCheck),
                                                                              _pollOrientationCheckTimer(_pollOrientationCheckDelegate, APP_ORIENTATION_MS),
                                                                              _orientationChangeListener(self, &ScreenManager::onOrientationChange),
                                                                              _cycleBacklightListener(self, &ScreenManager::onCycleBacklight),
                                                                              _colorChangeListener(self, &ScreenManager::onColorChange),
                                                                              _screenChangeListener(self, &ScreenManager::show),
                                                                              _showFatalErrorScreenListener(self, &ScreenManager::onShowFatalErrorScreen)
    {
    }

    ~Impl()
    {
    }

    Signal<unsigned short> _sendScreenChanged;

    AppContext *_context;
    OrientationManager _orientationMgr = OrientationManager(APP_ROTATION_THRESHOLD);
    unsigned int _currentScreen;

    unsigned short _lastForegroundColor;
    unsigned short _lastBackgroundColor;
    unsigned int _lastBrightness = 0;

    std::vector<Screen *> _screens;
    ErrorScreen _errorScreen = ErrorScreen(*_context);
    SplashScreen _splashScreen = SplashScreen(*_context);
    ConnectingScreen _connectScreen = ConnectingScreen(*_context);
    TallyScreen _tallyScreen = TallyScreen(*_context, HIGH_VIZ_MODE);
    SettingsScreen _settingsScreen = SettingsScreen(*_context);

    MethodSlot<ScreenManager, unsigned long> _pollForceRefreshDelegate;
    SlotLoopEvent _pollForceRefreshTimer;
    MethodSlot<ScreenManager, unsigned long> _pollOrientationCheckDelegate;
    SlotLoopEvent _pollOrientationCheckTimer;
    MethodSlot<ScreenManager, unsigned short> _orientationChangeListener;
    MethodSlot<ScreenManager, unsigned long> _cycleBacklightListener;
    MethodSlot<ScreenManager, Colors> _colorChangeListener;
    MethodSlot<ScreenManager, unsigned short> _screenChangeListener;
    MethodSlot<ScreenManager, const char *> _showFatalErrorScreenListener;
};

ScreenManager::ScreenManager(AppContext &context, unsigned int maxScreens)
    : _pimpl(new Impl(this, context, maxScreens))
{
}

ScreenManager::~ScreenManager()
{
    // this->sendOrientationChange.~Signal();
    _pimpl->_sendScreenChanged.~Signal();

    _pimpl->_orientationMgr.~OrientationManager();

    for (auto it = _pimpl->_screens.begin(); it != _pimpl->_screens.end(); ++it)
    {
        (*it)->unregister();
    }
    _pimpl->_screens.clear();

    delete _pimpl->_context;
}

void ScreenManager::begin()
{
    _pimpl->_orientationMgr.begin();

    _pimpl->_context->addLoopEvent(&_pimpl->_pollForceRefreshTimer);
    _pimpl->_context->addLoopEvent(&_pimpl->_pollOrientationCheckTimer);

    this->add(&(_pimpl->_errorScreen));
    this->add(&(_pimpl->_splashScreen));
    this->add(&(_pimpl->_connectScreen));
    this->add(&(_pimpl->_tallyScreen));
    this->add(&(_pimpl->_settingsScreen));
}

void ScreenManager::add(Screen *screen)
{
    screen->sendOrientationChange.attach(_pimpl->_orientationChangeListener);
    screen->sendCycleBacklight.attach(_pimpl->_cycleBacklightListener);
    screen->sendColorChange.attach(_pimpl->_colorChangeListener);
    screen->sendScreenChange.attach(_pimpl->_screenChangeListener);
    screen->sendShowFatalErrorScreen.attach(_pimpl->_showFatalErrorScreenListener);

    _pimpl->_sendScreenChanged.attach(screen->recvScreenChange);
    _pimpl->_screens[screen->getId()] = screen;
}

void ScreenManager::onOrientationChange(unsigned short orientation)
{
    // this->sendOrientationChange.fire(orientation);
    Serial.printf("DEBUG: ScreenManager::onOrientationChange(%d)\n", orientation);
    _pimpl->_orientationMgr.setOrientation(orientation);
}

void ScreenManager::onCycleBacklight(unsigned long timestamp)
{
    _pimpl->_pollForceRefreshTimer.setNextExecute(timestamp + APP_BRIGHTNESS_TIMEOUT_MS);

    char *brightnessString = new char[4];
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextDatum(MC_DATUM);

    // blit out previous message
    M5.Lcd.setTextColor(_pimpl->_lastBackgroundColor);
    sprintf(brightnessString, "%d%%", _pimpl->_lastBrightness);

    auto orientation = _pimpl->_orientationMgr.getOrientation();
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
    if (_pimpl->_currentScreen >= _pimpl->_screens.size())
    {
        return nullptr;
    }
    return _pimpl->_screens[_pimpl->_currentScreen];
}

void ScreenManager::show(unsigned short screenId)
{
    Serial.printf("DEBUG: show %d\n", screenId);
    _pimpl->_currentScreen = screenId;
    _pimpl->_sendScreenChanged.fire(screenId);
    _pimpl->_context->setCurrentScreen(screenId);
    auto screen = this->getCurrent();
    if (screen != nullptr)
    {
        screen->show();
    }
}

void ScreenManager::refresh()
{
    auto screen = this->getCurrent();
    if (screen != nullptr)
    {
        screen->refresh();
    }
}

void ScreenManager::handleInput(unsigned long timestamp, PinButton &m5Btn, PinButton &sideBtn)
{
    auto screen = this->getCurrent();
    if (screen != nullptr)
    {
        screen->handleInput(timestamp, m5Btn, sideBtn);
    }
}

void ScreenManager::pollForceRefresh(unsigned long timestamp)
{
    this->refresh();
}

void ScreenManager::pollOrientationCheck(unsigned long timestamp)
{
    unsigned int newRotation = _pimpl->_orientationMgr.checkRotationChange();

    if (newRotation != -1)
    {
        setRotation(newRotation);
    }
}

void ScreenManager::setOrientation(unsigned short orientation)
{
    _pimpl->_orientationMgr.setOrientation(orientation);
}

void ScreenManager::setRotation(byte newRotation)
{
    if (newRotation != _pimpl->_orientationMgr.getRotation())
    {
        _pimpl->_orientationMgr.setRotation(newRotation);
        this->refresh();
    }
}

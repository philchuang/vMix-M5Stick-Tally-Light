// hardware
#define ESP32
#include <M5StickC.h>

// libraries
#include <Callback.h>
#include <PinButton.h>
#include <string>

// app
#include "AppContext.h"
#include "OrientationManager.h"
#include "Screen.h"
#include "VmixManager.h"

class TallyScreen : public Screen
{
public:
    TallyScreen(AppContext &context, bool isHighViz, unsigned long tallyQuickPlayDelayMs) : Screen(this, context),
                                                       _isHighVizMode(isHighViz),
                                                       _tallyStateChangedListener(this, &TallyScreen::handleTallyStateChanged),
                                                       _tallyQuickplayDelegate(this, &TallyScreen::tallyQuickPlay),
                                                       _tallyQuickplayDelayEvent(_tallyQuickplayDelegate, -1),
                                                       _tallyQuickPlayDelayMs(tallyQuickPlayDelayMs)

    {
        this->_vmix = this->_context->getVmixManager();
        this->_vmix->onTallyStateChange.attach(this->_tallyStateChangedListener);
    }

    ~TallyScreen()
    {
    }

    unsigned int getId() { return SCREEN_TALLY; }

    void onScreenChanged(unsigned short screenId)
    {
        this->_isShowing = screenId == this->getId();
    }

    void show()
    {
        this->_isShowing = true;
        this->_isLandscape = true;
        auto settings = this->_context->getSettings();
        this->_vmix->setCurrentTallyNumber(settings->getVmixTally()); // also triggers refresh
    }

    void refresh()
    {
        Serial.printf("DEBUG: TallyScreen::refresh tally=%d, tallyState=%c\n", this->_tally, this->_tallyState);

        if (!this->_isShowing)
            return;

        switch (this->_tallyState)
        {
        case TALLY_NONE:
            renderTallyNone();
            break;
        case TALLY_SAFE:
            renderTallySafe();
            break;
        case TALLY_LIVE:
            renderTallyProgram();
            break;
        case TALLY_PRE:
            renderTallyPreview();
            break;
        default:
            renderTallyNone();
        }
    }

    void handleInput(unsigned long timestamp, PinButton &m5Btn, PinButton &sideBtn)
    {
        // TODO render quickplay delay indicator
        if (this->_tallyQuickPlayStartMillis != 0)
        {
            if (timestamp > this->_tallyQuickPlayEndMillis)
            {
                // TODO and clear indicator
                this->_tallyQuickPlayStartMillis = 0;
                this->_tallyQuickPlayEndMillis = 0;
            }
            else
            {
                // TODO render indicator as a percentage
                double pct = (timestamp - this->_tallyQuickPlayStartMillis) / this->_tallyQuickPlayDelayMs;
            }
        }

        if (this->_isLandscape)
        {
            if (m5Btn.isSingleClick())
            {
                Serial.println("DEBUG: TallyScreen::handleInput Changing orientation to portrait");
                this->_isLandscape = false;
                this->sendOrientationChange.fire(PORTRAIT);
                this->refresh();
            }
            else if (m5Btn.isLongClick())
            {
                this->sendCycleBacklight.fire(timestamp);
            }
            else if (sideBtn.isSingleClick())
            {
                // TODO think of another feature
            }
            else if (sideBtn.isLongClick())
            {
                this->_tallyQuickPlayStartMillis = timestamp;
                this->_tallyQuickPlayEndMillis = timestamp + this->_tallyQuickPlayDelayMs;
                this->_tallyQuickplayDelayEvent.setNextExecute(this->_tallyQuickPlayEndMillis);
            }
        }
        else
        {
            if (m5Btn.isSingleClick())
            {
                this->sendScreenChange.fire(SCREEN_SETTINGS);
            }
            else if (m5Btn.isLongClick())
            {
                this->sendCycleBacklight.fire(timestamp);
            }
            else if (sideBtn.isDoubleClick())
            {
                this->changeTally(this->_tally + 1);
            }
            else if (sideBtn.isLongClick())
            {
                this->changeTally(1);
            }
        }
    }

private:
    void changeTally(unsigned short newTally)
    {
        this->_tally = newTally;
        this->_context->getSettings()->setVmixTally(newTally);
        this->_context->saveSettings();
        this->_vmix->setCurrentTallyNumber(newTally);
        this->refresh();
    }

    void handleTallyStateChanged(char tallyState)
    {
        this->_tallyState = tallyState;
        this->_context->setTallyState(tallyState);
        this->_tally = this->_vmix->getCurrentTallyNumber();
        this->refresh();
    }

    void renderTallyText(const char *text)
    {
        this->sendOrientationChange.fire(LANDSCAPE);
        M5.Lcd.setTextSize(5);
        M5.Lcd.setTextDatum(MC_DATUM);
        M5.Lcd.drawString(text, 80, 40, 1);
    }

    void renderTallyNumber()
    {
        this->sendOrientationChange.fire(PORTRAIT);
        M5.Lcd.setTextSize(7);
        M5.Lcd.setTextDatum(MC_DATUM);
        char *text = new char[4];
        if (this->_tally < 10)
        {
            sprintf(text, "%d", this->_tally);
            M5.Lcd.drawString(text, 44, 80, 1);
        }
        else if (this->_tally < 100)
        {
            sprintf(text, "%d", this->_tally / 10);
            M5.Lcd.drawString(text, 44, 50, 1);
            sprintf(text, "%d", this->_tally % 10);
            M5.Lcd.drawString(text, 44, 110, 1);
        }
        delete[] text;
    }

    void renderTallyProgram()
    {
        setLedOn(this->_isHighVizMode);

        if (this->_isHighVizMode)
        {
            M5.Lcd.fillScreen(RED);
            this->sendColorChange.fire(Colors(WHITE, RED));
        }
        else
        {
            M5.Lcd.fillScreen(BLACK);
            this->sendColorChange.fire(Colors(RED, BLACK));
        }

        if (this->_isLandscape)
        {
            renderTallyText("LIVE");
        }
        else
        {
            renderTallyNumber();
        }
    }

    void renderTallyPreview()
    {
        setLedOn(false);

        if (this->_isHighVizMode)
        {
            M5.Lcd.fillScreen(GREEN);
            this->sendColorChange.fire(Colors(BLACK, GREEN));
        }
        else
        {
            M5.Lcd.fillScreen(BLACK);
            this->sendColorChange.fire(Colors(GREEN, BLACK));
        }

        if (this->_isLandscape)
        {
            renderTallyText("PRE");
        }
        else
        {
            renderTallyNumber();
        }
    }

    void renderTallySafe()
    {
        setLedOn(false);

        M5.Lcd.fillScreen(BLACK);
        this->sendColorChange.fire(Colors(WHITE, BLACK));

        if (this->_isLandscape)
        {
            renderTallyText("SAFE");
        }
        else
        {
            renderTallyNumber();
        }
    }

    void renderTallyNone()
    {
        setLedOn(false);

        if (this->_isHighVizMode)
        {
            M5.Lcd.fillScreen(YELLOW);
            this->sendColorChange.fire(Colors(BLACK, YELLOW));
        }
        else
        {
            M5.Lcd.fillScreen(BLACK);
            this->sendColorChange.fire(Colors(YELLOW, BLACK));
        }

        if (this->_isLandscape)
        {
            renderTallyText("????");
        }
        else
        {
            renderTallyNumber();
        }
    }

    void setLedOn(bool isOn)
    {
        this->_context->getBatteryManager()->setLedOn(isOn);
    }

    void tallyQuickPlay(unsigned long unused)
    {
        this->_vmix->sendQuickPlayInput(this->_tally);
    }

    bool _isHighVizMode;
    bool _isLandscape;
    bool _isShowing;
    MethodSlot<TallyScreen, char> _tallyStateChangedListener;
    MethodSlot<TallyScreen, unsigned long> _tallyQuickplayDelegate;
    SlotLoopEvent _tallyQuickplayDelayEvent;
    unsigned long _tallyQuickPlayDelayMs;
    unsigned long _tallyQuickPlayStartMillis;
    unsigned long _tallyQuickPlayEndMillis;

    VmixManager *_vmix;
    unsigned char _tallyState;
    unsigned short _tally;
};

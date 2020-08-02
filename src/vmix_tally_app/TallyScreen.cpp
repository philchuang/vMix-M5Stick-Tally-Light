#ifndef TALLYSCREEN_CPP
#define TALLYSCREEN_CPP

#define ESP32
#define LED_BUILTIN 10

#include "Screen.h"

#include <string>
#include <M5StickC.h>
#include <PinButton.h>
#include "OrientationManager.h"
#include "AppContext.h"
#include "VmixManager.h"

class TallyScreen : public Screen
{
public:
    TallyScreen(AppContext &context, bool isHighViz) : Screen(context), _isHighVizMode(isHighViz)
    {
        this->_vmix = this->_context->getVmixManager();
        // TODO hook into this->_vmix->onTallyStateChange - update tally state, and refresh if currently showing - how to determine?
    }

    ~TallyScreen()
    {
    }

    unsigned int getId() { return SCREEN_TALLY; }

    void show()
    {
        refresh();
    }

    void refresh()
    {
        this->_tallyState = this->_vmix->getCurrentTallyState();
        this->_tally = this->_vmix->getCurrentTallyNumber();

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
        if (this->_orientation == LANDSCAPE)
        {
            if (m5Btn.isSingleClick())
            {
                this->orientationChangeHandler.fire(PORTRAIT);
            }
            else if (m5Btn.isLongClick())
            {
                this->cycleBacklightHandler.fire(timestamp);
            }
            else if (sideBtn.isSingleClick())
            {
                // TODO think of another feature
            }
            else if (sideBtn.isLongClick())
            {
                this->_vmix->sendQuickPlayInput(this->_tally);
            }
        }
        else
        {
            if (m5Btn.isSingleClick())
            {
                this->screenChangeHandler.fire(SCREEN_SETTINGS);
            }
            else if (m5Btn.isLongClick())
            {
                this->cycleBacklightHandler.fire(timestamp);
            }
            else if (sideBtn.isDoubleClick())
            {
                this->_vmix->setCurrentTallyNumber(this->_tally + 1);
                refresh();
            }
            else if (sideBtn.isLongClick())
            {
                this->_vmix->setCurrentTallyNumber(1);
                refresh();
            }
        }
    }

private:
    void renderTallyText(const char *text)
    {
        this->orientationChangeHandler.fire(LANDSCAPE);
        M5.Lcd.setTextSize(5);
        M5.Lcd.setTextDatum(MC_DATUM);
        M5.Lcd.drawString(text, 80, 40, 1);
    }

    void renderTallyNumber()
    {
        this->orientationChangeHandler.fire(PORTRAIT);
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
            this->colorChangeHandler.fire(Colors(WHITE, RED));
        }
        else
        {
            M5.Lcd.fillScreen(BLACK);
            this->colorChangeHandler.fire(Colors(RED, BLACK));
        }

        if (this->_orientation == LANDSCAPE)
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
            this->colorChangeHandler.fire(Colors(BLACK, GREEN));
        }
        else
        {
            M5.Lcd.fillScreen(BLACK);
            this->colorChangeHandler.fire(Colors(GREEN, BLACK));
        }

        if (this->_orientation == LANDSCAPE)
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
        this->colorChangeHandler.fire(Colors(WHITE, BLACK));

        if (this->_orientation == LANDSCAPE)
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
            this->colorChangeHandler.fire(Colors(BLACK, YELLOW));
        }
        else
        {
            M5.Lcd.fillScreen(BLACK);
            this->colorChangeHandler.fire(Colors(YELLOW, BLACK));
        }

        if (this->_orientation == LANDSCAPE)
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
        if (isOn)
            digitalWrite(LED_BUILTIN, LOW);
        else
            digitalWrite(LED_BUILTIN, HIGH);
    }

    bool _isHighVizMode;
    unsigned short _orientation;
    VmixManager *_vmix;
    unsigned char _tallyState;
    unsigned short _tally;
};

#endif
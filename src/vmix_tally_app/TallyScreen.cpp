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
    }

    ~TallyScreen()
    {
    }

    unsigned int getId() { return SCREEN_TALLY; }

    void show()
    {
        this->_vmix = this->_context->getVmixManager();
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

    void handleInput(unsigned long timestamp, PinButton m5Btn, PinButton sideBtn)
    {
        // TODO implement
    }

private:
    void renderTallyText(const char *text)
    {
        if (this->orientationChangeHandler)
            this->orientationChangeHandler(LANDSCAPE);
        M5.Lcd.setTextSize(5);
        M5.Lcd.setTextDatum(MC_DATUM);
        M5.Lcd.drawString(text, 80, 40, 1);
    }

    void renderTallyNumber()
    {
        if (this->orientationChangeHandler)
            this->orientationChangeHandler(PORTRAIT);
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
            if (this->colorChangeHandler)
                this->colorChangeHandler(WHITE, RED);
        }
        else
        {
            M5.Lcd.fillScreen(BLACK);
            if (this->colorChangeHandler)
                this->colorChangeHandler(RED, BLACK);
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
            if (this->colorChangeHandler)
                this->colorChangeHandler(BLACK, GREEN);
        }
        else
        {
            M5.Lcd.fillScreen(BLACK);
            if (this->colorChangeHandler)
                this->colorChangeHandler(GREEN, BLACK);
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
        if (this->colorChangeHandler)
            this->colorChangeHandler(WHITE, BLACK);

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
            if (this->colorChangeHandler)
                this->colorChangeHandler(BLACK, YELLOW);
        }
        else
        {
            M5.Lcd.fillScreen(BLACK);
            if (this->colorChangeHandler)
                this->colorChangeHandler(YELLOW, BLACK);
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
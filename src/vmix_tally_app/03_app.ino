// intellisense support only, comment out before building
// #define ESP32
// #define LED_BUILTIN 10
// #define CLEAR_SETTINGS_ON_LOAD true
// #define PREPARE_BATTERY_LOGGING false
// #define TALLY_NONE ' '
// #define TALLY_NR_MAX 30
// #define SCREEN_START 0
// #define SCREEN_SETTINGS 1
// #define SCREEN_TALLY 2
// #define SCREEN_TALLY_NR 3
// #define SCREEN_ERROR 255
// #define VMIX_KEEPALIVE_MS 5000u
// #define VMIX_RESPONSE_MS 100u
// #define APP_ORIENTATION_MS 500u
// #define APP_SCREENREFRESH_MS 10000u
// #define M5_CHARGING_MS 5000u
// #define M5_BATTERYLEVEL_MS 30000u
// #define APP_BRIGHTNESS_TIMEOUT_MS 5000u
// #define APP_ROTATION_THRESHOLD 0.8f
// #define FONT 1
// #include <vector>
// #include <M5StickC.h>
// #include <SPIFFS.h>
// #include <WiFi.h>
// #include <PinButton.h>
// #include "AppSettings.h"
// #include "AppSettingsManager.h"
// #include "BatteryManager.h"
// #include "OrientationManager.h"
// #include "LoopEvent.h"
// #include "01_config.ino"
// #include "02_settings.ino"
// #include "04_wifi.ino"
// #include "05_vmix.ino"
// #include "99_utils.ino"
// AppSettings settings;
// AppSettingsManager settingsMgr;
// byte currentScreen;
// char currentTallyState;
// unsigned int conn_Reconnections;
// double currentBatteryLevel;
// bool isCharging;
// intellisense support only, comment out before building

BatteryManager battery;
OrientationManager orientationManager(APP_ROTATION_THRESHOLD);

PinButton btnM5 = PinButton(37);
PinButton btnSide = PinButton(39);

// char wifi_apDeviceName[32];  // global variable
// bool wifi_apEnabled = false; // global variable
// char wifi_apPass[64];        // global variable

bool wifi_isConnected = false;
bool vmix_isConnected = false;
unsigned int app_lastBrightness = 0;
unsigned short app_lastForegroundColor = WHITE;
unsigned short app_lastBackgroundColor = BLACK;
bool saveUptimeInfo = true;
std::vector<LoopEvent> loopEvents;
LoopEvent *screenRefreshCheck;

void setup()
{
  // power savings
  setCpuFrequencyMhz(80);
  btStop();

  // hardware initialization
  M5.begin();
  Serial.begin(115200);
  SPIFFS.begin();

  // services initialization
  battery.begin();
  if (HIGH_VIZ_MODE)
  {
    battery.setBacklight(100);
  }
  else
  {
    battery.setBacklight(60);
  }
  settingsMgr.begin();
  orientationManager.begin();

  // reset LED to off
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  if (CLEAR_SETTINGS_ON_LOAD)
  {
    settings_clear();
  }

  main_begin();
  main_splash();
  settings_load();

  if (LOG_BATTERY)
  {
    if (PREPARE_BATTERY_LOGGING)
    {
      settingsMgr.saveUptimeInfo(0, 0);
      saveUptimeInfo = false;
    }
    else
    {
      delay(10000);
    }
    if (settingsMgr.getLastUptime() != 0)
    {
      saveUptimeInfo = false;
    }
  }
  main_start();

  // server.on("/", handle_root);
  // server.begin();
  // Serial.println("HTTP server started.");
}

void main_begin()
{
  currentScreen = SCREEN_START;
  currentTallyState = TALLY_NONE;
  conn_Reconnections = 0;
  isCharging = false;
  currentBatteryLevel = 0;
  auto temp = LoopEvent(main_checkScreenRefresh, APP_SCREENREFRESH_MS);
  screenRefreshCheck = &temp;

  main_setupLoopListeners();
}

void main_setupLoopListeners()
{
  // TODO migrate these to SlotLoopEvent
  loopEvents.push_back(LoopEvent(main_checkIsCharging, M5_CHARGING_MS));
  loopEvents.push_back(LoopEvent(main_checkBatteryLevel, M5_BATTERYLEVEL_MS));
  loopEvents.push_back(LoopEvent(main_checkOrientation, APP_ORIENTATION_MS));
  loopEvents.push_back(*screenRefreshCheck);
  loopEvents.push_back(LoopEvent(main_handleButtons, 0));
  loopEvents.push_back(LoopEvent(main_pollVmix, VMIX_RESPONSE_MS));
  loopEvents.push_back(LoopEvent(main_pollKeepAlive, VMIX_KEEPALIVE_MS));
}

void main_splash()
{
  cls();
  main_updateOrientation(0);
  M5.Lcd.setTextDatum(MC_DATUM);
  M5.Lcd.setCursor(20, 15);
  M5.Lcd.drawString("vMix M5Stick-C Tally", 80, 15, FONT + 1);
  M5.Lcd.setCursor(35, 35);
  M5.Lcd.drawString("by Phil Chuang", 80, 35, FONT);
  M5.Lcd.setCursor(40, 50);
  M5.Lcd.drawString("& Guido Visser", 80, 55, FONT);
  delay(2000);
}

void main_start()
{
  cls();
  // sprintf(wifi_apDeviceName, "vMix_Tally_%d", settings.tallyNumber);
  // sprintf(wifi_apPass, "%s%s", wifi_apDeviceName, "_access");
  // Serial.printf("wifi_apDeviceName: %s\n", wifi_apDeviceName);
  // Serial.printf("wifi_apPass: %s\n", wifi_apPass);

  if (!main_reconnectWifi())
  {
    main_showErrorScreen("Could not connect to wifi!");
    vmix_isConnected = false;
    return;
  }

  if (!main_reconnectVmix())
  {
    main_showErrorScreen("Could not connect to vMix!");
    return;
  }

  vmix_showTallyScreen(settings.getVmixTally());
}

bool main_reconnectWifi()
{
  wifi_isConnected = false;

  if (!wifi_connect(settings.getWifiSsid(), settings.getWifiPassphrase()))
  {
    Serial.printf("Unable to connect to %s!\n", settings.getWifiSsid());
    main_updateOrientation(0);
    M5.Lcd.printf("Unable to connect to %s!\n", settings.getWifiSsid());
    return false;
  }

  wifi_isConnected = true;
  return true;
}

bool main_reconnectVmix()
{
  vmix_isConnected = false;
  currentTallyState = TALLY_NONE;
  vmix_renderTallyScreen(settings.getVmixTally());

  if (!wifi_isConnected)
  {
    if (!main_reconnectWifi())
    {
      return false;
    }
  }

  if (!vmix_connect(settings.getVmixAddress(), settings.getVmixPort()))
  {
    main_updateOrientation(0);
    Serial.println("Unable to connect to vMix!");
    M5.Lcd.println("Unable to connect to vMix!");
    return false;
  }

  vmix_isConnected = true;
  return true;
}

void loop()
{
  unsigned long timestamp = millis();

  // server.handleClient();

  for (auto it = loopEvents.begin(); it != loopEvents.end(); ++it)
  {
    if (!(it->execute(timestamp)))
    {
      return;
    }
  }
}

void main_restart()
{
  // server.close();

  Serial.println();
  Serial.println();
  Serial.println("------------");
  Serial.println("RESTART");
  Serial.println("------------");
  Serial.println();
  Serial.println();

  main_start();
}

void main_incrementTally()
{
  main_updateTally((settings.getVmixTally() % TALLY_NR_MAX) + 1);
}

void main_updateTally(unsigned short tally)
{
  settings.setVmixTally(tally);
  Serial.printf("Updated Tally to %d\n", settings.getVmixTally());
  settings_save();
  vmix_setTallyState(tally, TALLY_NONE);
  vmix_refreshTally();
}

bool main_checkIsCharging(unsigned long timestamp)
{
  isCharging = battery.isCharging();

  return true;
}

bool main_checkBatteryLevel(unsigned long timestamp)
{
  currentBatteryLevel = battery.getBatteryLevel();

  if (LOG_BATTERY && saveUptimeInfo)
  {
    settingsMgr.saveUptimeInfo(millis(), currentBatteryLevel);
  }

  return true;
}

bool main_checkOrientation(unsigned long timestamp)
{
  unsigned int newRotation = orientationManager.checkRotationChange();

  if (newRotation != -1)
  {
    main_updateRotation(newRotation);
  }

  return true;
}

void main_updateOrientation(byte newOrientation)
{
  main_updateOrientation(newOrientation, true);
}

void main_updateOrientation(byte newOrientation, bool force)
{
  orientationManager.setOrientation(newOrientation);

  unsigned int newRotation;
  newRotation = force ? orientationManager.checkRotationChange(0.0f) : orientationManager.checkRotationChange();

  if (newRotation != -1)
  {
    main_updateRotation(newRotation);
  }
}

void main_updateRotation(byte newRotation)
{
  if (newRotation != orientationManager.getRotation())
  {
    orientationManager.setRotation(newRotation);
    main_renderScreen();
  }
}

bool main_checkScreenRefresh(unsigned long timestamp)
{
  // TODO draw overlays like battery indicator
  main_renderScreen();

  return true;
}

bool main_handleButtons(unsigned long timestamp)
{
  // TODO create classes for each screen that handles showing, rendering, and input handling

  btnM5.update();
  btnSide.update();

  if (currentScreen == SCREEN_TALLY)
  {
    if (btnM5.isSingleClick())
    {
      vmix_showTallyNumberScreen(settings.getVmixTally());
    }
    else if (btnM5.isLongClick())
    {
      main_cycleBacklight(timestamp);
    }
    else if (btnSide.isSingleClick())
    {
      // TODO think of another feature
    }
    else if (btnSide.isLongClick())
    {
      vmix_quickPlayInput(settings.getVmixTally());
    }
  }
  else if (currentScreen == SCREEN_TALLY_NR)
  {
    if (btnM5.isSingleClick())
    {
      settings_showscreen();
    }
    else if (btnM5.isLongClick())
    {
      main_cycleBacklight(timestamp);
    }
    else if (btnSide.isDoubleClick())
    {
      main_incrementTally();
      vmix_showTallyNumberScreen(settings.getVmixTally());
    }
    else if (btnSide.isLongClick())
    {
      main_updateTally(1);
      vmix_showTallyNumberScreen(settings.getVmixTally());
    }
  }
  else if (currentScreen == SCREEN_SETTINGS)
  {
    if (btnM5.isSingleClick())
    {
      vmix_showTallyScreen(settings.getVmixTally());
    }
    else if (btnSide.isLongClick())
    {
      Serial.println("Swapping settings...");
      if (settings_swap())
      {
        Serial.println("Settings swapped!");
        main_restart();
        return false;
      }
    }
  }
  else if (currentScreen == SCREEN_ERROR)
  {
    if (btnM5.isLongClick())
    {
      main_restart();
      return false;
    }
    else if (btnSide.isLongClick())
    {
      settings_swap();
      main_restart();
      return false;
    }
    // else if (btnSide.isLongClick())
    // {
    //   settingsIdx = 1;
    //   settings_clear();
    //   settingsIdx = 0;
    //   settings_clear();
    //   settings_load();
    //   main_restart();
    //   return false;
    // }
  }

  return true;
}

void main_setScreenColors(unsigned short foregroundColor, unsigned short backgroundColor)
{
  app_lastForegroundColor = foregroundColor;
  app_lastBackgroundColor = backgroundColor;
  M5.Lcd.setTextColor(foregroundColor, backgroundColor);
}

void main_cycleBacklight(unsigned long timestamp)
{
  screenRefreshCheck->setNextExecute(timestamp + APP_BRIGHTNESS_TIMEOUT_MS);

  char *brightnessString = new char[4];
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextDatum(MC_DATUM);

  M5.Lcd.setTextColor(app_lastBackgroundColor);
  sprintf(brightnessString, "%d%%", app_lastBrightness);
  if (!orientationManager.getOrientation())
  {
    M5.Lcd.drawString(brightnessString, 80, 80, FONT);
  }
  else
  {
    M5.Lcd.drawString(brightnessString, 40, 160, FONT);
  }

  app_lastBrightness = battery.cycleBacklight();

  M5.Lcd.setTextColor(app_lastForegroundColor);
  sprintf(brightnessString, "%d%%", app_lastBrightness);
  if (!orientationManager.getOrientation())
  {
    M5.Lcd.drawString(brightnessString, 80, 80, FONT);
  }
  else
  {
    M5.Lcd.drawString(brightnessString, 40, 160, FONT);
  }
}

bool main_pollVmix(unsigned long timestamp)
{
  if (vmix_isConnected)
  {
    vmix_checkForResponses(settings.getVmixTally());
  }

  return true;
}

bool main_pollKeepAlive(unsigned long timestamp)
{
  if ((!wifi_isConnected && !vmix_isConnected) || currentScreen == SCREEN_ERROR)
  {
    return true;
  }

  if (!wifi_isAlive())
  {
    Serial.println("Disconnected from wifi, reconnecting...");
    if (!main_reconnectWifi())
    {
      main_showErrorScreen("Could not reconnect to wifi!");
      return false;
    }
  }

  if (!vmix_isAlive())
  {
    Serial.println("Disconnected from vMix, reconnecting...");
    conn_Reconnections++;
    if (!main_reconnectVmix())
    {
      main_showErrorScreen("Could not reconnect to vMix!");
      return false;
    }
  }

  return true;
}

void main_renderScreen()
{
  if (currentScreen == SCREEN_START)
  {
    // do nothing
  }
  else if (currentScreen == SCREEN_TALLY)
  {
    vmix_showTallyScreen(settings.getVmixTally());
  }
  else if (currentScreen == SCREEN_TALLY_NR)
  {
    vmix_showTallyNumberScreen(settings.getVmixTally());
  }
  else if (currentScreen == SCREEN_SETTINGS)
  {
    settings_renderscreen();
  }
  else if (currentScreen == SCREEN_ERROR)
  {
    // do nothing
  }
}

void main_showErrorScreen(const char *msg)
{
  currentScreen = SCREEN_ERROR;
  cls();
  main_updateOrientation(0);
  Serial.printf("ERR: %s\n", msg);
  M5.Lcd.printf("SETTINGS: %d/%d\n", settingsIdx + 1, MAX_SETTINGS_NR);
  M5.Lcd.println();
  M5.Lcd.println(msg);
  M5.Lcd.println();
  M5.Lcd.println("Hold M5 btn to restart.");
  M5.Lcd.println("Hold side btn to swap settings.");
  //M5.Lcd.println("TODO Hold both to hard reset.");
}
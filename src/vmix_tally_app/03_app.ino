// // intellisense support only, comment out before building
// #define ESP32
// #define LED_BUILTIN 10
// #define CLEAR_SETTINGS_ON_LOAD true
// #define TALLY_NONE ' '
// #define TALLY_NR_MAX 30
// #define SCREEN_START 0
// #define SCREEN_SETTINGS 1
// #define SCREEN_TALLY 2
// #define SCREEN_TALLY_NR 3
// #define SCREEN_ERROR 255
// #define VMIX_KEEPALIVE_MS 5000
// #define VMIX_RESPONSE_MS 100
// #define APP_ORIENTATION_MS 500
// #define APP_SCREENREFRESH_MS 10000
// #define M5_CHARGING_MS 5000
// #define M5_BATTERYLEVEL_MS 30000
// #define APP_BRIGHTNESS_TIMEOUT_MS 5000
// #define APP_ROTATION_THRESHOLD 0.8f
// #define FONT 1
// #include <M5StickC.h>
// #include <SPIFFS.h>
// #include <WiFi.h>
// #include <PinButton.h>
// #include "AppSettings.h"
// #include "AppSettingsManager.h"
// #include "BatteryManager.h"
// #include "OrientationManager.h"
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

BatteryManager battery = BatteryManager();
OrientationManager orientationManager = OrientationManager(APP_ROTATION_THRESHOLD);

PinButton btnM5 = PinButton(37);
PinButton btnSide = PinButton(39);

// char wifi_apDeviceName[32];  // global variable
// bool wifi_apEnabled = false; // global variable
// char wifi_apPass[64];        // global variable

bool wifi_isConnected = false;
bool vmix_isConnected = false;
unsigned int app_lastBrightness = 0;
unsigned long conn_NextKeepAliveCheck = 0;
unsigned long conn_NextVmixResponseCheck = 0;
unsigned long app_NextOrientationCheck = 0;
unsigned long app_NextScreenRefreshCheck = 0;
unsigned long m5_NextChargingCheck = 0;
unsigned long m5_NextBatteryLevelCheck = 0;
unsigned short app_lastForegroundColor = WHITE;
unsigned short app_lastBackgroundColor = BLACK;
bool saveUptimeInfo = true;

void setup()
{
  // power savings
  setCpuFrequencyMhz(80);
  btStop();

  // initialization
  M5.begin();
  Serial.begin(115200);
  SPIFFS.begin();

  battery.setBacklight(60);
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

  // TEMPORARY
  delay(10000);
  // settingsMgr.saveUptimeInfo(0,0);
  // saveUptimeInfo = false;
  if (settingsMgr.getLastUptime() != 0)
  {
    saveUptimeInfo = false;
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
  conn_NextVmixResponseCheck = 0;
  conn_NextKeepAliveCheck = 0;
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

  main_checkBatteryLevel(timestamp);
  main_checkOrientation(timestamp);
  main_checkScreenRefresh(timestamp);

  if (main_handleButtons(timestamp))
  {
    return;
  }

  main_pollVmix(timestamp);
  main_pollKeepAlive(timestamp);
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

void main_checkBatteryLevel(unsigned long timestamp)
{
  bool performChargingCheck = timestamp > m5_NextChargingCheck;
  if (performChargingCheck)
  {
    m5_NextChargingCheck = timestamp + M5_CHARGING_MS;
    isCharging = battery.isCharging();
  }
  bool performBatteryLevelCheck = timestamp > m5_NextBatteryLevelCheck;
  if (performBatteryLevelCheck)
  {
    m5_NextBatteryLevelCheck = timestamp + M5_BATTERYLEVEL_MS;
    currentBatteryLevel = battery.getBatteryLevel();

    // TEMPORARY
    if (saveUptimeInfo)
    {
      settingsMgr.saveUptimeInfo(millis(), currentBatteryLevel);
    }
  }
}

void main_checkOrientation(unsigned long timestamp)
{
  bool performOrientationCheck = timestamp > app_NextOrientationCheck;
  if (performOrientationCheck)
  {
    app_NextOrientationCheck = timestamp + APP_ORIENTATION_MS;
    unsigned int newRotation = orientationManager.checkRotationChange();

    if (newRotation != -1)
    {
      main_updateRotation(newRotation);
    }
  }
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

void main_checkScreenRefresh(unsigned long timestamp)
{
  // TODO draw overlays like battery indicator
  bool performScreenRefresh = timestamp > app_NextScreenRefreshCheck;
  if (performScreenRefresh)
  {
    app_NextScreenRefreshCheck = timestamp + APP_SCREENREFRESH_MS;
    main_renderScreen();
  }
}

bool main_handleButtons(unsigned long timestamp)
{
  bool breakLoop = false;

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
        breakLoop = true;
      }
    }
  }
  else if (currentScreen == SCREEN_ERROR)
  {
    if (btnM5.isLongClick())
    {
      main_restart();
      breakLoop = true;
    }
    else if (btnSide.isLongClick())
    {
      settings_swap();
      main_restart();
      breakLoop = true;
    }
    // else if (btnSide.isLongClick())
    // {
    //   settingsIdx = 1;
    //   settings_clear();
    //   settingsIdx = 0;
    //   settings_clear();
    //   settings_load();
    //   main_restart();
    //   breakLoop = true;
    // }
  }

  return breakLoop;
}

void main_setScreenColors(unsigned short foregroundColor, unsigned short backgroundColor)
{
  app_lastForegroundColor = foregroundColor;
  app_lastBackgroundColor = backgroundColor;
  M5.Lcd.setTextColor(foregroundColor, backgroundColor);
}

void main_cycleBacklight(unsigned long timestamp)
{
  app_NextScreenRefreshCheck = timestamp + APP_BRIGHTNESS_TIMEOUT_MS;

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

void main_pollVmix(unsigned long timestamp)
{
  if (vmix_isConnected)
  {
    bool performVmixResponseCheck = timestamp > conn_NextVmixResponseCheck;
    if (performVmixResponseCheck)
    {
      conn_NextVmixResponseCheck = timestamp + VMIX_RESPONSE_MS;
      vmix_checkForResponses(settings.getVmixTally());
    }
  }
}

void main_pollKeepAlive(unsigned long timestamp)
{
  if ((!wifi_isConnected && !vmix_isConnected) || currentScreen == SCREEN_ERROR)
  {
    return;
  }

  bool performKeepAliveCheck = timestamp > conn_NextKeepAliveCheck;
  if (performKeepAliveCheck)
  {
    conn_NextKeepAliveCheck = timestamp + VMIX_KEEPALIVE_MS;
    if (!wifi_isAlive())
    {
      Serial.println("Disconnected from wifi, reconnecting...");
      if (!main_reconnectWifi())
      {
        main_showErrorScreen("Could not reconnect to wifi!");
        return;
      }
    }

    if (!vmix_isAlive())
    {
      Serial.println("Disconnected from vMix, reconnecting...");
      conn_Reconnections++;
      if (!main_reconnectVmix())
      {
        main_showErrorScreen("Could not reconnect to vMix!");
        return;
      }
    }
  }
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
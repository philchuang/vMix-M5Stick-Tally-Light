// intellisense support only, comment out before building
// #define ESP32
// #define LED_BUILTIN 10
// #define EEPROM_SIZE 512
// #define TALLY_NONE ' '
// #define SCREEN_START 0
// #define SCREEN_SETTINGS 1
// #define SCREEN_TALLY 2
// #define SCREEN_TALLY_NR 3
// #define SCREEN_ERROR 255
// #define VMIX_KEEPALIVE_MS 5000
// #define VMIX_RESPONSE_MS 100
// #define APP_ORIENTATION_MS 500
// #include <HardwareSerial.h>
// #include <M5StickC.h>
// #include <SPIFFS.h>
// #include <EEPROM.h>
// #include <WiFi.h>
// #include <PinButton.h>
// #include "AppSettings.h"
// #include "01_config.ino"
// #include "02_settings.ino"
// #include "04_wifi.ino"
// #include "05_vmix.ino"
// #include "99_utils.ino"
// AppSettings settings = AppSettings(EEPROM_SIZE);
// byte currentScreen = SCREEN_START;
// char currentTallyState = TALLY_NONE;
// unsigned int conn_Reconnections = 0;
// intellisense support only, comment out before building

PinButton btnM5(37);
PinButton btnSide(39);

// char wifi_apDeviceName[32];  // global variable
// bool wifi_apEnabled = false; // global variable
// char wifi_apPass[64];        // global variable

bool imu_initialized = false;
bool wifi_isConnected = false;
bool vmix_isConnected = false;
unsigned long conn_NextKeepAliveCheck = 0;
unsigned long conn_NextVmixResponseCheck = 0;
unsigned long app_NextOrientationCheck = 0;

void setup()
{
  Serial.begin(115200);
  M5.begin();
  delay(10);
  M5.Lcd.setRotation(3);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  EEPROM.begin(EEPROM_SIZE);
  SPIFFS.begin();

  if (M5.MPU6886.Init() != 0)
  {
    Serial.println("MPU6886 error");
  }
  else
  {
    imu_initialized = true;
  }

  main_splash();
  main_start();

  // server.on("/", handle_root);
  // server.begin();
  // Serial.println("HTTP server started.");
}

void main_splash()
{
  M5.Lcd.setCursor(20, 15);
  M5.Lcd.println("vMix M5Stick-C Tally");
  M5.Lcd.setCursor(35, 35);
  M5.Lcd.println("by Guido Visser");
  M5.Lcd.setCursor(40, 55);
  M5.Lcd.println("& Phil Chuang");
  delay(2000);
}

void main_start()
{
  cls();
  settings_load();
  // sprintf(wifi_apDeviceName, "vMix_Tally_%d", settings.tallyNumber);
  // sprintf(wifi_apPass, "%s%s", wifi_apDeviceName, "_access");
  // Serial.printf("wifi_apDeviceName: %s\n", wifi_apDeviceName);
  // Serial.printf("wifi_apPass: %s\n", wifi_apPass);

  if (!main_reconnectWifi())
  {
    return;
  }

  if (!main_reconnectVmix())
  {
    return;
  }

  vmix_showTallyScreen(settings.getVmixTally());
}

bool main_reconnectWifi()
{
  wifi_isConnected = false;

  if (!wifi_connect(settings.getWifiSsid(), settings.getWifiPassphrase()))
  {
    Serial.printf("Unable to connect to %s\n!", settings.getWifiSsid());
    M5.Lcd.printf("Unable to connect to %s\n!", settings.getWifiSsid());
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

  main_checkOrientation(timestamp);

  if (!main_handleButtons())
  {
    return;
  }

  main_pollVmix(timestamp);
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

void main_checkOrientation(unsigned long timestamp)
{
  if (!imu_initialized)
  {
    return;
  }

  bool performOrientationCheck = timestamp > app_NextOrientationCheck + APP_ORIENTATION_MS;
  if (performOrientationCheck)
  {
    app_NextOrientationCheck = timestamp + APP_ORIENTATION_MS;

    float accX = 0;
    float accY = 0;
    float accZ = 0;

    M5.MPU6886.getAccelData(&accX, &accY, &accZ);

    if (accX > 0.8f)
    {
      main_updateOrientation(1);
    }
    else if (accX < -0.8f)
    {
      main_updateOrientation(3);
    }
  }
}

void main_updateOrientation(byte newRotation)
{
  byte currentRotation = M5.Lcd.getRotation();

  if (currentRotation == newRotation)
  {
    return;
  }

  M5.Lcd.setRotation(newRotation);
  main_renderScreen();
}

bool main_handleButtons()
{
  bool breakLoop = true;

  btnM5.update();
  btnSide.update();

  if (currentScreen == SCREEN_TALLY)
  {
    if (btnM5.isClick())
    {
      vmix_showTallyNumberScreen(settings.getVmixTally());
    }
    else if (btnSide.isClick())
    {
      // TODO think of another feature
    }
  }
  else if (currentScreen == SCREEN_TALLY_NR)
  {
    if (btnM5.isClick())
    {
      settings_showscreen();
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
    if (btnM5.isClick())
    {
      vmix_showTallyScreen(settings.getVmixTally());
    }
    // TODO change long press to load alternate settings
    else if (btnSide.isDoubleClick())
    {
      main_incrementTally();
      settings_renderscreen();
    }
    else if (btnSide.isLongClick())
    {
      main_updateTally(1);
      settings_renderscreen();
    }
  }
  else if (currentScreen == SCREEN_ERROR)
  {
    if (btnM5.isLongClick())
    {
      main_restart();
      breakLoop = false;
    }
    else if (btnSide.isLongClick())
    {
      settings_clear();
      settings_load();
      main_restart();
      breakLoop = false;
    }
  }

  return breakLoop;
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

    bool performKeepAliveCheck = timestamp > conn_NextKeepAliveCheck;
    if (performKeepAliveCheck)
    {
      conn_NextKeepAliveCheck = timestamp + VMIX_KEEPALIVE_MS;
      if (!vmix_isAlive())
      {
        Serial.println("Disconnected from vMix, reconnecting...");
        conn_Reconnections++;
        main_reconnectVmix();
      }
    }
  }
}

void main_renderScreen()
{
  if (currentScreen == SCREEN_TALLY)
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
    // does nothing
  }
}

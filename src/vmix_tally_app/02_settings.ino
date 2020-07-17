// intellisense support only, comment out before building
// #define ESP32
// #define EEPROM_SIZE 512
// #define MAX_SETTINGS_NR 2
// #define SCREEN_SETTINGS 1
// #include <HardwareSerial.h>
// #include <M5StickC.h>
// #include <WiFi.h>
// #include "AppSettings.h"
// #include "AppSettingsManager.h"
// #include "01_config.ino"
// #include "99_utils.ino"
// AppSettingsManager settingsMgr = AppSettingsManager(EEPROM_SIZE, MAX_SETTINGS_NR);
// byte settingsIdx = 0;
// AppSettings settings = AppSettings();
// byte currentScreen;
// unsigned int conn_Reconnections = 0;
// intellisense support only, comment out before building

// APPSETTINGS_SIZE * MAX_SETTINGS_NR cannot exceed EEPROM_SIZE
#ifdef SETTINGS1_WIFI_SSID
#define MAX_SETTINGS_NR 2
#else
#define MAX_SETTINGS_NR 1
#endif

AppSettingsManager settingsMgr = AppSettingsManager(EEPROM_SIZE, MAX_SETTINGS_NR);
byte settingsIdx = 0;

// Load settings from EEPROM
void settings_load()
{
  Serial.printf("Loading settings at %d...\n", settingsIdx);

  AppSettings s = settingsMgr.load(settingsIdx);
  if (!s.isValid())
  {
    Serial.println("Invalid settings found, using default settings.");
    if (settingsIdx == 0)
    {
      s.setWifiSsid(SETTINGS0_WIFI_SSID);
      s.setWifiPassphrase(SETTINGS0_WIFI_PASS);
      s.setVmixAddress(SETTINGS0_VMIX_ADDR);
      s.setVmixPort(SETTINGS0_VMIX_PORT);
      s.setVmixTally(SETTINGS0_TALLY_NR);
    }
#ifdef SETTINGS1_WIFI_SSID
    else if (settingsIdx == 1)
    {
      s.setWifiSsid(SETTINGS1_WIFI_SSID);
      s.setWifiPassphrase(SETTINGS1_WIFI_PASS);
      s.setVmixAddress(SETTINGS1_VMIX_ADDR);
      s.setVmixPort(SETTINGS1_VMIX_PORT);
      s.setVmixTally(SETTINGS1_TALLY_NR);
    }
#endif
    settingsMgr.save(settingsIdx, s);
    settings = s;
    Serial.println("Default settings:");
    settings_print();
  }
  else
  {
    settings = s;
    Serial.println("Settings loaded.");
    settings_print();
  }
}

void settings_clear()
{
  Serial.println("Clearing EEPROM...");

  settingsMgr.clear(settingsIdx);

  Serial.println("EEPROM cleared.");
}

// Save settings to EEPROM
void settings_save()
{
  Serial.println("Saving settings...");

  settingsMgr.save(settingsIdx, settings);

  Serial.println("Settings saved.");
}

// Print settings
void settings_print()
{
  Serial.printf("SSID: %s\n", settings.getWifiSsid());
  Serial.printf("Pass: %s\n", settings.getWifiPassphrase());
  Serial.printf("vMix: %s\n", settings.getVmixAddressWithPort());
  Serial.printf("Tally: %d\n", settings.getVmixTally());
}

void settings_showscreen()
{
  currentScreen = SCREEN_SETTINGS;
  settings_renderscreen();
}

void settings_renderscreen()
{
  cls();
  main_updateOrientation(0);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.printf("SETTINGS: %d/%d\n", settingsIdx+1, MAX_SETTINGS_NR);
  M5.Lcd.printf("-SSID: %s\n", settings.getWifiSsid());
  M5.Lcd.print ("-IP: ");
  M5.Lcd.println(WiFi.localIP());
  M5.Lcd.printf("-vMix: %s\n", settings.getVmixAddressWithPort());
  M5.Lcd.printf("-TALLY: %d\n", settings.getVmixTally());
  M5.Lcd.printf("Reconnections: %u\n", conn_Reconnections);
  unsigned long timestamp = millis();
  unsigned long hours = timestamp / 1000 / 60 / 60;
  unsigned long minutes = (timestamp - (hours * 1000 * 60 * 60)) / 1000 / 60;
  unsigned long seconds = (timestamp - (hours * 1000 * 60 * 60) - (minutes * 1000 * 60)) / 1000;
  M5.Lcd.printf("Runtime: %02u:%02u:%02u\n", hours, minutes, seconds);
  M5.Lcd.println();
  M5.Lcd.println("Hold side btn to swap settings.");
}

bool settings_swap()
{
  settingsIdx = (settingsIdx + 1) % MAX_SETTINGS_NR;
  settings_load();
  return settings.isValid();
}
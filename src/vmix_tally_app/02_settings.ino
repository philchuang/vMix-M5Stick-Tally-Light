// intellisense support only, comment out before building
// #define ESP32
// #define EEPROM_SIZE 512
// #define SCREEN_SETTINGS 1
// #include <HardwareSerial.h>
// #include <M5StickC.h>
// #include <WiFi.h>
// #include "AppSettings.h"
// #include "01_config.ino"
// #include "99_utils.ino"
// byte currentScreen;
// unsigned int conn_Reconnections = 0;
// intellisense support only, comment out before building

// Load settings from EEPROM
void settings_load()
{
  Serial.println("Loading settings...");

  if (!settings.load())
  {
    Serial.println("Invalid settings found, using default settings.");
    Serial.println("Loaded settings:");
    settings_print();
    settings.setWifiSsid(WIFI_SSID);
    settings.setWifiPassphrase(WIFI_PASS);
    settings.setVmixAddress(VMIX_ADDR);
    settings.setVmixPort(VMIX_PORT);
    settings.setVmixTally(TALLY_NR);
    settings.save();
    Serial.println("Default settings:");
    settings_print();
  }
  else
  {
    Serial.println("Settings loaded.");
    settings_print();
  }
}

void settings_clear()
{
  Serial.println("Clearing EEPROM...");
  
  settings.clear();

  Serial.println("EEPROM cleared.");
}

// Save settings to EEPROM
void settings_save()
{
  Serial.println("Saving settings...");

  settings.save();

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

void settings_showscreen() {
  currentScreen = SCREEN_SETTINGS;
  settings_renderscreen();
}

void settings_renderscreen() {
  cls();
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.printf("SSID: %s\n", settings.getWifiSsid());
  M5.Lcd.print("IP: ");
  M5.Lcd.println(WiFi.localIP());
  M5.Lcd.printf("vMix: %s\n", settings.getVmixAddressWithPort());
  M5.Lcd.printf("TALLY: %d\n", settings.getVmixTally());
  M5.Lcd.printf("Reconnections: %u\n", conn_Reconnections);
  M5.Lcd.println();
  M5.Lcd.println("Dbl-click side btn to increment Tally.");
  M5.Lcd.println("Hold side btn to clear settings.");
  // TODO change long press to load alternate settings
}
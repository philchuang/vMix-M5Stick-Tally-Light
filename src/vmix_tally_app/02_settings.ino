// Constants
const byte WifiSsidMaxLength = 64;
const byte WifiPassMaxLength = 64;
const byte VmixAddrMaxLength = 64;
const byte TallyNumberMaxValue = 16;

// Settings object
struct Settings
{
  char wifi_ssid[WifiSsidMaxLength];
  char wifi_pass[WifiPassMaxLength];
  char vmix_addr[VmixAddrMaxLength];
  unsigned short vmix_port;
  unsigned short tallyNumber;
};

// Default settings object
Settings defaultSettings = {
  WIFI_SSID,
  WIFI_PASS,
  VMIX_IP,
  VMIX_PORT,
  TALLY_NR
};

Settings settings = defaultSettings; // GLOBAL

// Load settings from EEPROM
void settings_load()
{
  Serial.println("Loading settings...");

  unsigned long ptr = 0;

  for (int i = 0; i < WifiSsidMaxLength; i++)
  {
    settings.wifi_ssid[i] = EEPROM.read(ptr++);
  }

  for (int i = 0; i < WifiPassMaxLength; i++)
  {
    settings.wifi_pass[i] = EEPROM.read(ptr++);
  }

  for (int i = 0; i < VmixAddrMaxLength; i++)
  {
    settings.vmix_addr[i] = EEPROM.read(ptr++);
  }

  settings.vmix_port = (EEPROM.read(ptr++) << 8) + EEPROM.read(ptr++);

  settings.tallyNumber = (EEPROM.read(ptr++) << 8) + EEPROM.read(ptr++);

  if (strlen(settings.wifi_ssid) == 0 || strlen(settings.wifi_pass) == 0
      || strlen(settings.vmix_addr) == 0 || settings.vmix_port == 0u 
      || settings.tallyNumber == 0u)
  {
    Serial.println("Invalid settings found, using default settings.");
    settings = defaultSettings;
    settings_save();
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
  unsigned long ptr = 0;
  for (int i = 0; i < EEPROM_SIZE; i++)
  {
    EEPROM.write(ptr++, 0);
  }

  EEPROM.commit();
  Serial.println("EEPROM cleared.");
}

// Save settings to EEPROM
void settings_save()
{
  Serial.println("Saving settings...");

  settings_clear();

  unsigned long ptr = 0;

  for (int i = 0; i < WifiSsidMaxLength; i++)
  {
    EEPROM.write(ptr++, settings.wifi_ssid[i]);
  }

  for (int i = 0; i < WifiPassMaxLength; i++)
  {
    EEPROM.write(ptr++, settings.wifi_pass[i]);
  }

  for (int i = 0; i < VmixAddrMaxLength; i++)
  {
    EEPROM.write(ptr++, settings.vmix_addr[i]);
  }

  EEPROM.write(ptr++, (byte) (settings.vmix_port >> 8));
  EEPROM.write(ptr++, (byte) (settings.vmix_port & 0xFF));

  EEPROM.write(ptr++, (byte) (settings.tallyNumber >> 8));
  EEPROM.write(ptr++, (byte) (settings.tallyNumber & 0xFF));

  EEPROM.commit();

  Serial.println("Settings saved.");
  settings_print();
}

// Print settings
void settings_print()
{
  Serial.println("SETTINGS: ");
  Serial.printf("SSID: %s\n", settings.wifi_ssid);
  M5.Lcd.printf("SSID: %s\n", settings.wifi_ssid);
  Serial.printf("Pass: %s\n", settings.wifi_pass);
  M5.Lcd.printf("Pass: %s\n", settings.wifi_pass);
  Serial.printf("vMix: %s:%u\n", settings.vmix_addr, settings.vmix_port);
  M5.Lcd.printf("vMixr: %s:%u\n", settings.vmix_addr, settings.vmix_port);
  Serial.printf("Tally: %d\n", settings.tallyNumber);
  M5.Lcd.printf("Tally: %d\n", settings.tallyNumber);
}

void settings_showscreen() {
  currentScreen = SCREEN_SETTINGS;
  settings_renderscreen();
}

void settings_renderscreen() {
  cls();
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.printf("SSID: %s\n", settings.wifi_ssid);
  M5.Lcd.print("IP: ");
  M5.Lcd.println(WiFi.localIP());
  M5.Lcd.printf("vMix: %s:%u\n", settings.vmix_addr, settings.vmix_port);
  M5.Lcd.printf("TALLY: %d\n", settings.tallyNumber);
  M5.Lcd.printf("Reconnections: %u\n", conn_Reconnections);
  M5.Lcd.println();
  M5.Lcd.println("Dbl-click side btn to increment Tally.");
  M5.Lcd.println("Hold side btn to clear settings.");
}
WiFiClient vmix_client; // global variable
// WebServer server(80);  // global variable

PinButton btnM5(37);
PinButton btnSide(39);

// char wifi_apDeviceName[32];  // global variable
bool wifi_apEnabled = false; // global variable
// char wifi_apPass[64];        // global variable

const unsigned long conn_KeepAliveCheckMs = 5000; // move to global constant
unsigned long conn_NextKeepAliveCheck = 0;
const unsigned long conn_VmixResponseCheckMs = 100; // move to global constant
unsigned long conn_NextVmixResponseCheck = 0;

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

  vmix_showTallyScreen();
}

bool main_reconnectWifi()
{
  wifi_isConnected = false;

  if (!wifi_connect())
  {
    Serial.printf("Unable to connect to %s\n!", settings.wifi_ssid);
    M5.Lcd.printf("Unable to connect to %s\n!", settings.wifi_ssid);
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
  vmix_renderTallyScreen();

  if (!wifi_isConnected) {
    if (!main_reconnectWifi()) {
      return false;
    }
  }

  if (!vmix_connect())
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

  btnM5.update();
  btnSide.update();

  if (currentScreen == SCREEN_TALLY)
  {
    if (btnM5.isClick())
    {
      settings_showscreen();
    }
    else if (btnSide.isClick())
    {
      vmix_showTallyNumberScreen();
    }
  }
  else if (currentScreen == SCREEN_TALLY_NR)
  {
    if (btnM5.isClick())
    {
      settings_showscreen();
    }
    else if (btnSide.isClick())
    {
      vmix_showTallyScreen();
    }
  }
  else if (currentScreen == SCREEN_SETTINGS)
  {
    if (btnM5.isClick())
    {
      vmix_showTallyScreen();
    }
    else if (btnSide.isDoubleClick())
    {
      settings.tallyNumber = (settings.tallyNumber % TALLY_NR_MAX) + 1; // increment the tally number
      Serial.printf("Updated Tally to %d\n", settings.tallyNumber);
      settings_save();
      settings_renderscreen();
      vmix_refreshTally();
    }
    else if (btnSide.isLongClick())
    {
      settings.tallyNumber = 1;
      settings_save();
      settings_renderscreen();
      vmix_refreshTally();
    }
  }
  else if (currentScreen == SCREEN_ERROR)
  {
    if (btnM5.isLongClick())
    {
      main_restart();
      return;
    }
    else if (btnSide.isLongClick())
    {
      settings_clear();
      settings_load();
      main_restart();
      return;
    }
  }

  if (vmix_isConnected)
  {
    bool performVmixResponseCheck = timestamp > conn_NextVmixResponseCheck;
    if (performVmixResponseCheck)
    {
      conn_NextVmixResponseCheck = timestamp + conn_VmixResponseCheckMs;
      if (vmix_client.available())
      {
        String data = vmix_client.readStringUntil('\r\n');
        vmix_handleData(data);
      }
    }

    bool performKeepAliveCheck = timestamp > conn_NextKeepAliveCheck;
    if (performKeepAliveCheck)
    {
      conn_NextKeepAliveCheck = timestamp + conn_KeepAliveCheckMs;
      if (!wifi_apEnabled && !vmix_client.available() && !vmix_client.connected())
      {
        Serial.println("Disconnected from vMix, reconnecting...");
        conn_Reconnections++;
        main_reconnectVmix();
      }
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

void main_showCurrentScreen()
{
  if (currentScreen == SCREEN_TALLY)
  {
    vmix_showTallyScreen();
  }
  else if (currentScreen == SCREEN_SETTINGS)
  {
    settings_showscreen();
  }
}

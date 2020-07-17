// intellisense support only, comment out before building
// #define ESP32
// #include <HardwareSerial.h>
// #include <M5StickC.h>
// #include <WiFi.h>
// #include "99_utils.ino"
// intellisense support only, comment out before building

bool wifi_connect(const char *ssid, const char *passphrase)
{
  Serial.printf("SSID: %s\n", ssid);
  Serial.printf("Pass: %s\n", passphrase);
  Serial.printf("Connecting to %s with %s...", ssid, passphrase);

  cls();
  main_updateOrientation(0);
  M5.Lcd.setTextSize(1);
  M5.Lcd.print("Connecting to WiFi...");

  if (WiFi.status() == WL_CONNECTED)
  {
    WiFi.disconnect();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, passphrase);

  byte timeout = 10;
  while (WiFi.status() != WL_CONNECTED && timeout > 0)
  {
    delay(1000);
    timeout--;
    Serial.print(".");
    M5.Lcd.print(".");
  }
  Serial.println();
  M5.Lcd.println();

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("Connected!");
    M5.Lcd.println("Connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    return true;
  }

  if (WiFi.status() == WL_IDLE_STATUS)
    Serial.println("Idle");
  else if (WiFi.status() == WL_NO_SSID_AVAIL)
    Serial.println("No SSID Available");
  else if (WiFi.status() == WL_SCAN_COMPLETED)
    Serial.println("Scan Completed");
  else if (WiFi.status() == WL_CONNECT_FAILED)
    Serial.println("Connection Failed");
  else if (WiFi.status() == WL_CONNECTION_LOST)
    Serial.println("Connection Lost");
  else if (WiFi.status() == WL_DISCONNECTED)
    Serial.println("Disconnected");
  else
    Serial.println("Unknown Failure");

  // wifi_apStart();
  return false;
}

bool wifi_isAlive()
{
  return WiFi.status() == WL_CONNECTED;
}

// void wifi_apStart()
// {
//   cls();
//   Serial.println("AP Start...");
//   Serial.printf("AP SSID: %s\n", wifi_apDeviceName);
//   Serial.printf("AP pass: %s\n", wifi_apPass);
//   M5.Lcd.println("AP Start...");
//   M5.Lcd.printf("AP SSID: %s\n", wifi_apDeviceName);
//   M5.Lcd.printf("AP pass: %s\n", wifi_apPass);

//   WiFi.mode(WIFI_AP);
//   WiFi.softAP(wifi_apDeviceName, wifi_apPass);
//   delay(100);
//   Serial.printf("IP: ");
//   Serial.println(WiFi.softAPIP());

//   wifi_apEnabled = true;
// }

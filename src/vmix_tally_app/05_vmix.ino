// intellisense support only, comment out before building
// #define ESP32
// #define VMIX_CONN_RETRIES 3
// #define SCREEN_START 0
// #define SCREEN_TALLY 2
// #define SCREEN_TALLY_NR 3
// #define SCREEN_ERROR 255
// #define TALLY_NONE ' '
// #define TALLY_SAFE '0'
// #define TALLY_LIVE '1'
// #define TALLY_PRE '2'
// #define FONT 1
// #include <HardwareSerial.h>
// #include <M5StickC.h>
// #include <WiFi.h>
// #include "01_config.ino"
// #include "02_settings.ino"
// #include "03_app.ino"
// #include "99_utils.ino"
// byte currentScreen = SCREEN_START;
// char currentTallyState = TALLY_NONE;
// intellisense support only, comment out before building

#define VMIX_API_SUBSCRIBE_TALLY "SUBSCRIBE TALLY\r\n"
#define VMIX_API_SUBSCRIBE_TALLY_RESPONSE_PREFIX "SUBSCRIBE OK TALLY Subscribed"
#define VMIX_API_GET_TALLY "TALLY\r\n"
#define VMIX_API_GET_TALLY_RESPONSE_PREFIX "TALLY OK "
#define VMIX_API_GET_VERSION_RESPONSE_PREFIX "VERSION OK "
#define VMIX_API_FUNCTION_QUICKPLAY_INPUT "FUNCTION QuickPlay Input=%d\r\n"
#define VMIX_API_FUNCTION_QUICKPLAY_INPUT_RESPONSE "FUNCTION OK COMPLETED"

WiFiClient vmix_client;

// Connect to vMix instance
bool vmix_connect(const char *addr, unsigned short port)
{
  cls();
  main_updateOrientation(0);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(WHITE, BLACK);
  Serial.println("Connecting to vMix...");
  M5.Lcd.println("Connecting to vMix...");

  if (vmix_client.connected())
  {
    vmix_client.stop();
  }

  byte retryAttempt = 0;
  byte maxRetry = VMIX_CONN_RETRIES;
  do
  {
    if (!vmix_client.connect(addr, port))
    {
      if (retryAttempt == maxRetry)
      {
        return false;
      }

      retryAttempt++;
      Serial.printf("Retrying %d/%d...\n", retryAttempt, maxRetry);
      M5.Lcd.printf("Retrying %d/%d...\n", retryAttempt, maxRetry);
      delay(1000);

      continue;
    }

    Serial.println("Connection opened.");
    Serial.println("Subscribing to tally events...");
    vmix_client.print(VMIX_API_SUBSCRIBE_TALLY);

    return true;
  } while (true);

  return false;
}

bool vmix_isAlive()
{
  return vmix_client.connected() || vmix_client.available();
}

void vmix_refreshTally()
{
  if (vmix_client.connected())
  {
    vmix_client.print(VMIX_API_GET_TALLY);
  }
}

void vmix_quickPlayInput(unsigned short tally)
{
  if (vmix_client.connected())
  {
    vmix_client.printf(VMIX_API_FUNCTION_QUICKPLAY_INPUT, tally);
  }
}

void vmix_checkForResponses(unsigned short tally)
{
  if (!vmix_client.available())
  {
    return;
  }

  String data = vmix_client.readStringUntil('\r\n');
  Serial.printf("VMIX: %s\n", data.c_str());

  if (data.indexOf(VMIX_API_GET_TALLY_RESPONSE_PREFIX) == 0)
  {
    char newState = data.charAt(tally + 8); // response is like "TALLY OK 00000000"
    Serial.printf("tally #%u state: %c\n", tally, newState);
    vmix_setTallyState(tally, newState);
  }
  else if (data.indexOf(VMIX_API_GET_VERSION_RESPONSE_PREFIX) == 0)
  {
    Serial.println("vMix connection established.");
  }
  else if (data.indexOf(VMIX_API_SUBSCRIBE_TALLY_RESPONSE_PREFIX) == 0)
  {
    Serial.println("Tally subscription created.");
  }
  else if (data.indexOf(VMIX_API_FUNCTION_QUICKPLAY_INPUT_RESPONSE) == 0)
  {
    Serial.println("Function QuickPlay successful.");
  }
}

void vmix_setTallyState(unsigned short tally, char newState)
{
  // Check if tally state has changed
  if (currentTallyState == newState)
  {
    return;
  }
  Serial.printf("updating tally state: %c to %c\n", currentTallyState, newState);
  currentTallyState = newState;
  vmix_renderTallyScreen(tally);
}

void vmix_renderTallyText(const char *text)
{
  main_updateOrientation(0);
  M5.Lcd.setTextSize(5);
  M5.Lcd.setTextDatum(MC_DATUM);
  M5.Lcd.drawString(text, 80, 40, FONT);
}

void vmix_renderTallyNumber(unsigned char tally)
{
  main_updateOrientation(1);
  M5.Lcd.setTextSize(7);
  M5.Lcd.setTextDatum(MC_DATUM);
  char *text = new char[4];
  if (tally < 10)
  {
    sprintf(text, "%d", tally);
    M5.Lcd.drawString(text, 44, 80, FONT);
  }
  else if (tally < 100)
  {
    sprintf(text, "%d", tally / 10);
    M5.Lcd.drawString(text, 44, 50, FONT);
    sprintf(text, "%d", tally % 10);
    M5.Lcd.drawString(text, 44, 110, FONT);
  }
  delete[] text;
}

void vmix_renderTallyProgram(unsigned short tally)
{
  digitalWrite(LED_BUILTIN, LOW);
  if (currentScreen != SCREEN_TALLY && currentScreen != SCREEN_TALLY_NR)
  {
    return;
  }

  M5.Lcd.fillScreen(RED);
  M5.Lcd.setTextColor(WHITE, RED);
  if (currentScreen == SCREEN_TALLY)
  {
    vmix_renderTallyText("LIVE");
  }
  else if (currentScreen == SCREEN_TALLY_NR)
  {
    vmix_renderTallyNumber(tally);
  }
}

void vmix_renderTallyPreview(unsigned short tally)
{
  digitalWrite(LED_BUILTIN, HIGH);
  if (currentScreen != SCREEN_TALLY && currentScreen != SCREEN_TALLY_NR)
  {
    return;
  }

  M5.Lcd.fillScreen(GREEN);
  M5.Lcd.setTextColor(BLACK, GREEN);
  if (currentScreen == SCREEN_TALLY)
  {
    vmix_renderTallyText("PRE");
  }
  else if (currentScreen == SCREEN_TALLY_NR)
  {
    vmix_renderTallyNumber(tally);
  }
}

void vmix_renderTallySafe(unsigned short tally)
{
  digitalWrite(LED_BUILTIN, HIGH);
  if (currentScreen != SCREEN_TALLY && currentScreen != SCREEN_TALLY_NR)
  {
    return;
  }

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE, BLACK);
  if (currentScreen == SCREEN_TALLY)
  {
    vmix_renderTallyText("SAFE");
  }
  else if (currentScreen == SCREEN_TALLY_NR)
  {
    vmix_renderTallyNumber(tally);
  }
}

void vmix_renderTallyNone(unsigned short tally)
{
  digitalWrite(LED_BUILTIN, HIGH);
  if (currentScreen != SCREEN_TALLY && currentScreen != SCREEN_TALLY_NR)
  {
    return;
  }

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(YELLOW, BLACK);
  if (currentScreen == SCREEN_TALLY)
  {
    vmix_renderTallyText("????");
  }
  else if (currentScreen == SCREEN_TALLY_NR)
  {
    vmix_renderTallyNumber(tally);
  }
}

void vmix_showTallyScreen(unsigned short tally)
{
  currentScreen = SCREEN_TALLY;
  vmix_renderTallyScreen(tally);
}

void vmix_showTallyNumberScreen(unsigned short tally)
{
  currentScreen = SCREEN_TALLY_NR;
  vmix_renderTallyScreen(tally);
}

void vmix_renderTallyScreen(unsigned short tally)
{
  switch (currentTallyState)
  {
  case TALLY_NONE:
    vmix_renderTallyNone(tally);
    break;
  case TALLY_SAFE:
    vmix_renderTallySafe(tally);
    break;
  case TALLY_LIVE:
    vmix_renderTallyProgram(tally);
    break;
  case TALLY_PRE:
    vmix_renderTallyPreview(tally);
    break;
  default:
    vmix_renderTallyNone(tally);
  }
}
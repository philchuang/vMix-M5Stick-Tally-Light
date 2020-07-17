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

#define VMIX_API_SUBSCRIBE_TALLY "SUBSCRIBE TALLY"
#define VMIX_API_SUBSCRIBE_TALLY_RESPONSE_PREFIX "SUBSCRIBE OK TALLY Subscribed"
#define VMIX_API_GET_TALLY "TALLY"
#define VMIX_API_GET_TALLY_RESPONSE_PREFIX "TALLY OK "
#define VMIX_API_GET_VERSION_RESPONSE_PREFIX "VERSION OK "

WiFiClient vmix_client;

// Connect to vMix instance
bool vmix_connect(const char *addr, unsigned short port)
{
  cls();
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
        cls();
        settings_print();
        M5.Lcd.println();
        M5.Lcd.println("Could not connect!");
        M5.Lcd.println("Hold M5 btn to restart.");
        M5.Lcd.println("Hold side btn to reset.");
        currentScreen = SCREEN_ERROR;
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
    vmix_client.println(VMIX_API_SUBSCRIBE_TALLY);

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
    vmix_client.println(VMIX_API_GET_TALLY);
  }
}

void vmix_checkForResponses(unsigned short tally)
{
  if (!vmix_client.available())
  {
    return;
  }

  String data = vmix_client.readStringUntil('\r\n');

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

void vmix_renderTallyProgram(unsigned short tally)
{
  digitalWrite(LED_BUILTIN, LOW);
  if (currentScreen != SCREEN_TALLY && currentScreen != SCREEN_TALLY_NR)
  {
    return;
  }

  cls();
  M5.Lcd.setTextSize(5);
  M5.Lcd.fillScreen(RED);
  M5.Lcd.setTextColor(WHITE, RED);
  if (currentScreen == SCREEN_TALLY)
  {
    M5.Lcd.setCursor(25, 23);
    M5.Lcd.println("LIVE");
  }
  else if (currentScreen == SCREEN_TALLY_NR)
  {
    M5.Lcd.setCursor(23, 23);
    M5.Lcd.println(tally);
  }
}

void vmix_renderTallyPreview(unsigned short tally)
{
  digitalWrite(LED_BUILTIN, HIGH);
  if (currentScreen != SCREEN_TALLY && currentScreen != SCREEN_TALLY_NR)
  {
    return;
  }

  cls();
  M5.Lcd.setTextSize(5);
  M5.Lcd.fillScreen(GREEN);
  M5.Lcd.setTextColor(BLACK, GREEN);
  if (currentScreen == SCREEN_TALLY)
  {
    M5.Lcd.setCursor(40, 23);
    M5.Lcd.println("PRE");
  }
  else if (currentScreen == SCREEN_TALLY_NR)
  {
    M5.Lcd.setCursor(23, 23);
    M5.Lcd.println(tally);
  }
}

void vmix_renderTallySafe(unsigned short tally)
{
  digitalWrite(LED_BUILTIN, HIGH);
  if (currentScreen != SCREEN_TALLY && currentScreen != SCREEN_TALLY_NR)
  {
    return;
  }

  cls();
  M5.Lcd.setTextSize(5);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE, BLACK);
  if (currentScreen == SCREEN_TALLY)
  {
    M5.Lcd.setCursor(23, 23);
    M5.Lcd.println("SAFE");
  }
  else if (currentScreen == SCREEN_TALLY_NR)
  {
    M5.Lcd.setCursor(23, 23);
    M5.Lcd.println(tally);
  }
}

void vmix_renderTallyNone(unsigned short tally)
{
  digitalWrite(LED_BUILTIN, HIGH);
  if (currentScreen != SCREEN_TALLY && currentScreen != SCREEN_TALLY_NR)
  {
    return;
  }

  cls();
  M5.Lcd.setTextSize(5);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(YELLOW, BLACK);
  if (currentScreen == SCREEN_TALLY)
  {
    M5.Lcd.setCursor(23, 23);
    M5.Lcd.println("?");
  }
  else if (currentScreen == SCREEN_TALLY_NR)
  {
    M5.Lcd.setCursor(23, 23);
    M5.Lcd.println(tally);
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
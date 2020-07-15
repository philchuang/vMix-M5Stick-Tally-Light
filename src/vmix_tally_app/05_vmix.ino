// Connect to vMix instance
bool vmix_connect()
{
  cls();
  M5.Lcd.setTextSize(1);
  Serial.println("Connecting to vMix...");
  M5.Lcd.println("Connecting to vMix...");

  if (vmix_client.connected())
  {
    vmix_client.stop();
  }

  const char *addr = settings.vmix_addr;
  unsigned short port = settings.vmix_port;

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
    vmix_client.println("SUBSCRIBE TALLY");

    return true;
  } while (true);

  return false;
}

void vmix_refreshTally()
{
  if (vmix_client.connected())
  {
    vmix_client.println("TALLY");
  }
}

// Handle incoming data
void vmix_handleData(String &data)
{
  if (data.indexOf("TALLY OK ") == 0)
  {
    char newState = data.charAt(settings.tallyNumber + 8); // response is like "TALLY OK 00000000"
    Serial.printf("tally #%u state: %c\n", settings.tallyNumber, newState);
    // Check if tally state has changed
    if (currentTallyState != newState)
    {
      Serial.printf("updating tally state: %c to %c\n", currentTallyState, newState);
      currentTallyState = newState;
      vmix_renderTallyScreen();
    }
  }
  else if (data.indexOf("VERSION OK ") == 0)
  {
    Serial.println("vMix connection established.");
  }
  else if (data.indexOf("SUBSCRIBE OK TALLY Subscribed") == 0)
  {
    Serial.println("Tally subscription created.");
  }
}

void vmix_renderTallyProgram()
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
    M5.Lcd.println(settings.tallyNumber);
  }
}

void vmix_renderTallyPreview()
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
    M5.Lcd.println(settings.tallyNumber);
  }
}

void vmix_renderTallySafe()
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
    M5.Lcd.println(settings.tallyNumber);
  }
}

void vmix_showTallyScreen()
{
  currentScreen = SCREEN_TALLY;
  vmix_renderTallyScreen();
}

void vmix_showTallyNumberScreen()
{
  currentScreen = SCREEN_TALLY_NR;
  vmix_renderTallyScreen();
}

void vmix_renderTallyScreen()
{
  switch (currentTallyState)
  {
  case TALLY_SAFE:
    vmix_renderTallySafe();
    break;
  case TALLY_LIVE:
    vmix_renderTallyProgram();
    break;
  case TALLY_PRE:
    vmix_renderTallyPreview();
    break;
  default:
    vmix_renderTallySafe();
  }
}
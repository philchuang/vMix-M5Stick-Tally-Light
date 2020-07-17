/* TODOs
  - OOP-ify
  - centered text
  - automatic orientation flipping
  - battery indicator
  - if in preview, button press to quickplay input
*/

#define ESP32

#include <stdio.h>
#include <string.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <WebServer.h>

#include <AXP192.h>
#include <IMU.h>
#include <M5StickC.h>
#include <RTC.h>

#include <PinButton.h>

#include "AppSettings.h"

#define LED_BUILTIN 10
#define EEPROM_SIZE 512

#define SCREEN_ERROR 255
#define SCREEN_START 0
#define SCREEN_SETTINGS 1
#define SCREEN_TALLY 2
#define SCREEN_TALLY_NR 3

#define TALLY_NONE ' '
#define TALLY_SAFE '0'
#define TALLY_LIVE '1'
#define TALLY_PRE '2'

#define VMIX_CONN_RETRIES 3

// GLOBAL STATE
char currentTallyState = TALLY_NONE;
byte currentScreen = SCREEN_START;
bool wifi_isConnected = false;
bool vmix_isConnected = false;
unsigned int conn_Reconnections = 0;

# vMix M5Stick-C Tally Light

## Tally in action

(Click images to play video)

### Overview

[![Alt text](https://img.youtube.com/vi/AwrGejqj_YY/0.jpg)](https://youtu.be/AwrGejqj_YY)

### Tally statuses

[![Alt text](https://img.youtube.com/vi/anjMjVwFqTU/0.jpg)](https://youtu.be/anjMjVwFqTU)

### Changing Tally Input

[![Alt text](https://img.youtube.com/vi/vNC3jXKfyJU/0.jpg)](https://youtu.be/vNC3jXKfyJU)

## Getting started

### What you need

The **M5StickC** is an ESP32-powered unit, programmable with Arduino IDE. Built-in 80x160 color screen, LED, wifi, bluetooth, IR sensor, microphone, accelerometer, battery. It also has connectors for additional modules to extend its capabilities.

It's [$10 direct from the manufacturer](https://m5stack.com/collections/m5-core/products/stick-c), but it will take a few weeks to arrive.

Or if you can't wait, you can buy it for [$20 from Amazon](https://www.amazon.com/dp/B07R48W8R8/ref=cm_sw_r_tw_dp_x_oFNeFbQN387ED).

### Step 1

1. Follow the arduino tutorial on the [M5Stack website](https://docs.m5stack.com/#/en/arduino/arduino_development).
    1. Instead of Arduino IDE you can also use [Visual Studio Code](https://code.visualstudio.com/) with the [Arduino extension](https://marketplace.visualstudio.com/items?itemName=vsciot-vscode.vscode-arduino).
1. Copy the libraries that are used by this project (found in the `~\libs` folder) to `%userprofile%\Documents\Arduino\libraries`.

### Step 2

Copy `~\src\vmix_tally_app\01_config.ino.template` as `01_config.ino` and change the values to match your setup. The second set of settings are optional, uncomment and update them if needed.

```c
#define SETTINGS0_WIFI_SSID "your_wifi1"
#define SETTINGS0_WIFI_PASS "wifi_pass1"
#define SETTINGS0_VMIX_ADDR "192.168.1.100" // or hostname
#define SETTINGS0_VMIX_PORT 8099 // default TCP API port
#define SETTINGS0_TALLY_NR 1 // initial tally number
```

### Step 3

Using the IDE, compile and upload to your M5StickC.

## USAGE

1. Turn on device
1. Wait for wifi and vmix connection
1. The first screen is the tally status
    1. `SAFE`: input not in preview or live
    1. `PRE`: input in preview
    1. `LIVE`: input is live
    1. `????`: tally status unavailable
    1. M5 button single-click: display the tally number screen
    1. M5 button long-click: cycle screen brightness
    1. Side button long-click: QuickPlay this input
1. The second screen is the tally number
    1. M5 button single-click: display the connection settings screen
    1. M5 button long-click: cycle screen brightness
    1. Side button double-click: increment the tally number
    1. Side button long-click: reset tally number to 1
1. The third screen is the settings
    1. SSID, IP, vMix IP/port, Runtime.
    1. M5 button single-click: display the tally status screen
    1. Side button long-click: load next settings and restart

## TODO

* [ ]  Battery indicator on all screens
* [ ]  Detect and indicate when battery is fully charged
* [ ]  Decrease battery usage / confirm minimum working voltage
* [ ]  Use Preferences.h instead of EEPROM.h for settings
* [ ]  Restore last used settings on boot (instead of just #1) OR ability to select settings on boot
* [ ]  Continued cleanup and refactor of logic - event-driven model
* [ ]  Hard-reset capability from settings or error screen
* [ ]  Access Point mode when not connected to a WiFi network
* [ ]  Webserver for easy configuration

## Thanks

This project is a fork of [Guido Visser's vMix-M5Stick-Tally-Light](https://github.com/guido-visser/vMix-M5Stick-Tally-Light), which inspired me and got the ball rolling.
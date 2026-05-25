#  Plant Guard System

[![Language](https://img.shields.io/badge/language-C++%2011-red)](https://isocpp.org/)
[![Platform](https://img.shields.io/badge/platform-Arduino-blue)](https://www.arduino.cc)

An Arduino-based smart plant guardian that detects cats, monitors for falls, and reports via Telegram — built for the **Arduino MKR IoT Carrier**.

---

##  Overview

Plant Guard keeps your plants safe by:
- Detecting cats nearby through a **PIR sensor**
- Sounding a randomized **buzzer alarm** and flashing **LEDs** when a cat is detected
- Detecting if the plant **falls or is violently hit** via the onboard IMU accelerometer
- Sending **Telegram notifications** to the owner for both events
- Exposing a set of **bot commands** to control the system remotely

---

## 🛠 Hardware Requirements

| Component | Details |
|---|---|
| Board | Arduino MKR WiFi 1010 |
| Shield | Arduino MKR IoT Carrier |
| Sensor | PIR motion sensor (connected to pin `A6`) |
| Connectivity | Wi-Fi (via `WiFiNINA`) |

---

## 📦 Dependencies

Install the following libraries via the Arduino Library Manager or PlatformIO:

| Library | Purpose |
|---|---|
| `Arduino_MKRIoTCarrier` | Carrier hardware abstraction (display, LEDs, buzzer, IMU, env sensor) |
| `WiFiNINA` | Wi-Fi connectivity |
| `UniversalTelegramBot` | Telegram Bot API client |
| `ArduinoJson` | Required by UniversalTelegramBot |
| `ezTime` | NTP time sync with automatic timezone and DST support |
|`EEPROM`   | Required by ezTime|
|`mbed`|Required by EEPROM|

---

## 📁 Project Structure

```
PlantGuard_MKR/
├── include/
│   ├── CatAlarm.h          # Alarm logic: buzzer, LEDs, fall detection, display
│   ├── telegramBot.h       # Telegram bot: command handling, message formatting
│   ├── testHardware.h      # Hardware self-test routines
│   └── secrets.h           # ⚠️ Sensitive credentials (not committed to VCS)
├── src/
│   ├── main.cpp            # Entry point: setup, loop, PIR reading, system state
│   ├── CatAlarm.cpp
│   ├── telegramBot.cpp
│   └── testHardware.cpp
├── .gitignore
├── platformio.ini          # PlatformIO build configuration
└── README.md
```

---

## 🔐 Configuration — `secrets.h`

Create a `secrets.h` file in the `include/` folder with the following content:

```cpp
#pragma once

#define SSID_WIFI     "your_wifi_ssid"
#define PASSWORD_WIFI "your_wifi_password"
#define BOT_TOKEN     "your_telegram_bot_token"
#define USER_ID       "your_telegram_chat_id"
```

> ⚠️ **Never commit `secrets.h` to version control.** Add it to `.gitignore`.

To obtain a bot token, talk to [@BotFather](https://t.me/BotFather) on Telegram.  
To find your chat ID, talk to [@userinfobot](https://t.me/userinfobot).

---

## 🤖 Telegram Bot Commands

| Command | Description |
|---|---|
| `/help` | Show all available commands |
| `/active_guard` | Activate the plant guard system |
| `/stop_guard` | Deactivate the plant guard system |
| `/test` | Run all hardware self-tests |
| `/plant_health` | Get current temperature and humidity readings with timestamp |

The bot includes a **security layer**: only messages from the configured `USER_ID` are processed; all others are silently rejected.

---

## ⚙️ System Behaviour

### Cat Detection
When the PIR sensor reads HIGH:
- The display shows a cat ASCII art message
- The alarm triggers: LEDs turn yellow, the buzzer plays **7 randomized beeps**
- After the 7th beep the alarm stops automatically and the system resumes monitoring
- While the alarm is playing the PIR is ignored — no double triggers

### Fall Detection
The IMU continuously monitors the squared magnitude of the acceleration vector:
- **Freefall** → magnitude drops below `0.15 g²`
- **Impact / violent shake** → magnitude spikes above `4.0 g²`

On detection:
1. Two Telegram alerts are sent to the owner
2. The display shows a warning message
3. The alarm triggers automatically
4. The system deactivates itself (`isArduinoActive = false`)

### Alarm (`CatAlarm`)
- Beep durations and intervals are **randomized** for a less predictable alarm
- LEDs turn **yellow** when triggered, **green** when deactivated
- The alarm is non-blocking: managed via `millis()` in `update()`
- `catDetected` is passed by reference to `CatAlarm` — when the alarm finishes it resets the flag automatically, no extra logic needed in `main.cpp`

---

## 🌐 Timezone

Arduino has no real-time clock — every time it boots, its internal clock resets to zero. To solve this, the system syncs with an **NTP (Network Time Protocol) server** on boot: a public internet server that provides the exact current time (accurate to milliseconds, sourced from atomic clocks).

The library used is **ezTime**, which handles sync, timezone, and DST automatically:

```cpp
waitForSync();                  // blocks until NTP sync is complete
timeZone.setLocation("geoip");  // detects timezone from the board's public IP
```

`waitForSync()` queries `pool.ntp.org` — a pool of thousands of free public NTP servers worldwide. Once synced, the board knows the exact UTC time.

`setLocation("geoip")` then asks a GeoIP service which timezone corresponds to the board's IP address, and loads the correct DST rules automatically. No timezone string or offset needs to be configured manually — it works anywhere in the world.

`events()` is called every loop iteration: it keeps the internal clock aligned with the NTP server in the background, correcting any drift over time.

Timestamps appear in all Telegram messages and `/plant_health` reports in the **local time of the network the board is connected to**, with summer/winter time handled automatically.

---

## 💾 Hardware Self-Test

The `/test` command runs `run_all_hardware_tests()`, which sequentially tests:

| Test | Type | What it checks |
|---|---|---|
| Display | Non-atomic | Fills screen blue, then clears |
| Temperature/Humidity | Atomic | Reads ENV sensor, validates range (-20 °C to 80 °C) |
| Gyroscope | Atomic | Verifies IMU is available and readable |
| LEDs | Non-atomic | Briefly lights all 5 LEDs white |
| Buzzer | Non-atomic | Emits two short beeps |

> Total test delay is ~650 ms, safely within the 1 s bot polling interval, so no messages are missed during testing.

---

##  🎬 Getting Started

1. Wire the PIR sensor signal pin to `A6` on the MKR board
2. Install all required libraries (including `ezTime`)
3. Create `secrets.h` with your credentials
4. Compile and upload via PlatformIO
5. Open the Serial Monitor at `9600 baud` to follow boot and system logs
6. Message your bot on Telegram with `/active_guard` to start monitoring

---

## 📝 Notes

- `CARRIER_CASE = false` must be set **before** `carrier.begin()` — otherwise the sensors fail to initialize
- The system starts with `isArduinoActive = false` — it must be explicitly activated via `/active_guard`
- The display is only rewritten when the state changes (`isScreenUpdated` flag) to avoid unnecessary SPI traffic
- `alarm.update()` is called inside `if(isArduinoActive)` — if the system is deactivated mid-alarm the buzzer stops immediately

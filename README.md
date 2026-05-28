#  Plant Guard System

[![Language](https://img.shields.io/badge/language-C++%2011-red)](https://en.cppreference.com/cpp/11)
[![Platform](https://img.shields.io/badge/platform-Arduino-blue)](https://www.arduino.cc)
[![License](https://img.shields.io/badge/license-MIT-yellow)](https://github.com/Franc7Behemoth66/PlantGuard_Arduino/tree/main?tab=MIT-1-ov-file)


An Arduino-based smart plant guardian that detects cats, monitors for falls, and reports via Telegram — built for the **Arduino MKR IoT Carrier**.

---
## 🎯What this code does?

Plant Guard protects your plants by detecting cats nearby using a PIR sensor. When a cat is spotted, it sounds a randomised buzzer alarm and flashes LEDs. The onboard IMU accelerometer also detects falls or violent hits, sending Telegram notifications to the owner for both events. Additionally, a set of bot commands allows remote control of the system.

## 🛠 Hardware Requirements

| Component | Details |
|---|---|
| Board | [Arduino MKR WiFi 1010](https://docs.arduino.cc/hardware/mkr-wifi-1010/) |
| Shield | [Arduino MKR IoT Carrier](https://docs.arduino.cc/tutorials/mkr-iot-carrier/mkr-iot-carrier-01-technical-reference/#display) |
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
| `ArduinoJson` |Required by UniversalTelegramBot, converts JSON API responses into data structures|
| `ezTime` | NTP time sync with automatic timezone and DST support |

---

## 📁 Project Structure

```
PlantGuard_MKR/
├── include/
│   ├── CatAlarm.h          # Alarm logic: buzzer, LEDs, fall detection, display
│   ├── telegramBot.h       # Telegram bot: command handling, message formatting
│   ├── testHardware.h      # Hardware self-test routines
│   └── secrets.h           # ⚠️ Sensitive credentials 
├── src/
│   ├── main.cpp            # Entry point: setup, loop, PIR reading, system state
│   ├── CatAlarm.cpp
│   ├── telegramBot.cpp
│   └── testHardware.cpp
├── .gitignore
├── LICENSE                 # MIT license
├── patch_libs.py           # Patches incompatible library headers at build time
├── platformio.ini          # PlatformIO build configuration
└── README.md
```

---
## 🩹 Library Patches — `patch_libs.py`

Two of the dependencies ship with bugs or incompatibilities with the SAMD21 hardware. `patch_libs.py` is a PlatformIO pre-build script that patches the library source files automatically before compilation, so no manual edits are needed.

**Patch 1 — ezTime: disable EEPROM cache**  
ezTime by default tries to cache timezone data in EEPROM. The MKR WiFi 1010 has no EEPROM, which causes the board to stall on boot. The script comments out `#define EZTIME_CACHE_EEPROM` in `ezTime.h` to disable this behaviour.

**Patch 2 — Arduino_MKRIoTCarrier: fix `NONE` enum collision**  
The carrier library declares a `NONE = -1` enum in the global namespace, which collides with identifiers from other libraries and causes ODR (One Definition Rule) compilation errors. The script renames it to `SHANE_NONE` via regex to resolve the conflict.

Both patches are idempotent — running them multiple times has no effect. They are triggered automatically by PlatformIO at the start of every build via `env.AddPreAction("compile", ...)`, so they also handle cases where libraries are re-downloaded mid-project.

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

> ⚠️ This file must stay on device, and it shouldn't be add to git or pushed on a public repo. In order to do so, `src/secrets.h` is already listed in `.gitignore`.

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
| `/time_zone <offset>` | Set timezone as UTC offset (e.g. `/time_zone 2` for UTC+2) |

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

Arduino has no real-time clock — every time it boots, its internal clock resets to zero. To solve this, the system syncs with an **NTP server** on boot: a public internet server that provides the exact current time (accurate to milliseconds, sourced from atomic clocks).

The library used is **ezTime**:

```cpp
waitForSync(); // blocks until UTC time is obtained from pool.ntp.org
```

Once synced, the board knows the exact UTC time. The timezone is then set manually by the user via Telegram: on every boot the bot sends a message showing the current UTC time and asking the user to set their local offset:

```
🕒 Current Time (UTC): 14:32
Please set your timezone using: /time_zone <hours>, e.g. /time_zone 2
```

The `/time_zone` command accepts any UTC offset between -12 and +14 and applies it immediately using a POSIX string:

```cpp
// /time_zone 2  →  UTC-2  (ezTime uses reversed sign convention)
_tz->setPosix("UTC-2");
```

`events()` is called every loop iteration to keep the internal clock aligned with the NTP server, correcting any drift over time.

All timestamps in Telegram messages and `/plant_health` reports reflect the user-configured local time.

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
4. Run `patch_libs.py` if required (fixes incompatible library headers)
5. Compile and upload via PlatformIO
6. Open the Serial Monitor at `9600 baud` to follow boot and system logs
7. Reply to the bot's startup message with `/time_zone <offset>` to set your local time
8. Message your bot on Telegram with `/active_guard` to start monitoring

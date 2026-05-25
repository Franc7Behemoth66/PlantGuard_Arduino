
# 🪴 Plant Guard System
[![Language](https://img.shields.io/badge/language-C++%2011-red)](https://en.cppreference.com/cpp/11)
[![Platform](https://img.shields.io/badge/platform-Arduino-blue)](https://www.arduino.cc)

An Arduino-based smart plant guardian that detects cats, monitors for falls, and reports via Telegram — built for the **Arduino MKR IoT Carrier**.

---

## 📋 Overview

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
| Shield | [Arduino MKR IoT Carrier](https://docs.arduino.cc/hardware/mkr-iot-carrier/) |
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
├── platformio.ini          # PlatformIO build configuration
└── README.md
```

---

## 🔐 Configuration — `secrets.h`

Create a `secrets.h` file in `root/include` with the following content:

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
| `/plant_health` | Get current temperature and humidity readings |

The bot includes a **security layer**: only messages from the configured `USER_ID` are processed; all others are silently rejected.

---

## ⚙️ System Behaviour

### Cat Detection
When the PIR sensor reads HIGH:
- The display shows a cat ASCII art message
- The alarm triggers: LEDs turn yellow, the buzzer plays randomized beeps
- The alarm continues playing until manually deactivated via `/stop_guard`

### Fall Detection
The IMU continuously monitors the squared magnitude of the acceleration vector:
- **Freefall** → magnitude drops below `0.15 g²`
- **Impact / violent shake** → magnitude spikes above `4.0 g²`

On detection:
1. A Telegram alert is sent to the owner
2. The display shows a warning message
3. The alarm triggers automatically
4. The system deactivates itself (`isArduinoActive = false`)

### Alarm (`CatAlarm`)
- Beep durations and intervals are **randomized** for a less predictable (more annoying) alarm
- LEDs turn **yellow** when triggered, **green** when deactivated (the color choice is based on the visual spectrum of cats)
- The alarm state is non-blocking: managed via `millis()` in `update()`

---

## 🧪 Hardware Self-Test

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

## 🌐 Timezone Handling

The bot formats message timestamps in **Italian local time**, automatically switching between:
- **CET** (UTC+1) in winter
- **CEST** (UTC+2) in summer

⚠️ If your timezone is different from UTC+1 or UTC+2 (based on the season), you have to rewrite the function  ***bool _isCest(time_t t)***  in `telegramBot.cpp` in order to manage properly the hours added to the variable *utc* (type: time_t) in the fn ***_formatTimestamp*** in `telegramBot.cpp`  .



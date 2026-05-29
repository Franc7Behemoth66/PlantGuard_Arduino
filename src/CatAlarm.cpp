#include "CatAlarm.h"

CatAlarm::CatAlarm(MKRIoTCarrier& carrier, bool& isSysTrigger)
    : _carrier(carrier), _isTriggered(isSysTrigger), _fallDetector(true),
      _beepCounter(0), _lastBeepTime(0), _lastBlinkTime(0),
      _soundStatus(false), _blinkStatus(false) {}

void CatAlarm::begin() {
    _carrier.display.fillScreen(ST77XX_BLACK);
    _carrier.leds.fill(_carrier.leds.Color(0, 55, 0), 0, 5); // green: system ready
    _carrier.leds.show();
}

void CatAlarm::trigger(bool active) {
    if (active) {
        if (!_isTriggered) {
            _isTriggered = true;
            _beepCounter = 0;
            _soundStatus = false;  
            _blinkStatus = false;  
            _lastBeepTime = millis();
            _lastBlinkTime = millis();
        }
    } else {
        _isTriggered = false;
        _carrier.Buzzer.noSound();
        _carrier.leds.fill(_carrier.leds.Color(0, 55, 0), 0, 5); // back to green
        _carrier.leds.show();
    }
}

void CatAlarm::update() {
    if (!_isTriggered) return;

    long int currentTime = millis();

    if (currentTime - _lastBlinkTime >= 300) {
        _blinkStatus = !_blinkStatus;
        if (_blinkStatus)
            _carrier.leds.fill(_carrier.leds.Color(255, 255, 0), 0, 5);
        else
            _carrier.leds.clear();
        _carrier.leds.show();
        _lastBlinkTime = currentTime;
    }

    // --- Buzzer beeps ---
    if (!_soundStatus && ((currentTime - _lastBeepTime) >= random(200) + 100)) {
        _carrier.Buzzer.sound(random(4500) + 170);
        _soundStatus = true;
        _lastBeepTime = currentTime;
    } else if (_soundStatus && ((currentTime - _lastBeepTime) >= random(400) + 100)) {
        _carrier.Buzzer.noSound();
        _soundStatus = false;
        _beepCounter++;
        _lastBeepTime = currentTime;
    }

    if (_beepCounter >= 7) {
        trigger(false);
    }
}

bool CatAlarm::fallDetector(bool active) {
    _fallDetector = active;
    if (_fallDetector) {
        float x, y, z;
        if (_carrier.IMUmodule.accelerationAvailable()) {
            _carrier.IMUmodule.readAcceleration(x, y, z);
            float A = (x * x) + (y * y) + (z * z);
            if (A < 0.15 || A > 4.0) {
                Serial.println("[ALARM] Fall or violent impact detected!");
                updateDisplay("FALL DETECTED!", ST77XX_RED, 3);
                return true;
            }
            return false;
        }
        return false;
    }
    return false;
}

void CatAlarm::updateDisplay(const char* message, uint16_t color, int font_size) {
    _carrier.display.fillScreen(ST77XX_BLACK);
    _carrier.display.setTextColor(color);
    _carrier.display.setTextSize(font_size);
    _carrier.display.setCursor(20, 100);
    _carrier.display.print(message);
}

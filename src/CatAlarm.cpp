#include "CatAlarm.h" 
CatAlarm::CatAlarm(MKRIoTCarrier& carrier) : _carrier(carrier), _isTriggered(false), _fallDetector(true), _nextDelay(0), _beepCounter(0),
    _lastBeepTime(0){}

void CatAlarm::begin() {
    _carrier.display.fillScreen(ST77XX_BLACK);
    _carrier.leds.clear();
    _carrier.leds.show(); 
}

void CatAlarm::trigger(bool active) {
    if (active) {
        if (!_isTriggered) { 
            _carrier.leds.fill(_carrier.leds.Color(255, 255, 0), 0, 5);
            _carrier.leds.show(); 
            
            _isTriggered = true;
            _beepCounter = 0;               
            _lastBeepTime = millis();     
            _nextDelay = 0;               
        }
    } 
    else {
        _isTriggered = false; // Spegne la macchina a stati
        _carrier.leds.fill(_carrier.leds.Color(0, 55, 0), 0, 5);
        _carrier.leds.show();
    }
}

void CatAlarm::update() {
    if (_isTriggered) {
        if (millis() - _lastBeepTime >= _nextDelay) {
        
            if (_beepCounter < 7) {
                double freq_note = random(1000) + 4000;
                double period_note = random(700) + 130;
                _nextDelay = random(350) + 10 + period_note; 
                _carrier.Buzzer.beep(freq_note, period_note);
                
                _lastBeepTime = millis(); 
                _beepCounter++; 
            } 
            else 
                trigger(false); 
        }
    }
}

bool CatAlarm::fallDetector(bool active) {
    _fallDetector = active;
    if (!_fallDetector) return; 
    float x, y, z;
    if (_carrier.IMUmodule.accelerationAvailable()) {
        _carrier.IMUmodule.readAcceleration(x, y, z);
        // Calculate the squared magnitude of the acceleration vector.
        // use power of itself rather the sqrt() of itself.
        float A = (x * x) + (y * y) + (z * z);
        // A resting board outputs a squared vector of 1.0 (1g * 1g).
        // 1. Freefall condition: the value drops drastically (approaching 0g).
        // 2. Shock/Impact condition: the value spikes well above normal gravity (> 2g).
        if (A < 0.15 || A > 4.0) {
            Serial.println("[ALARM] Fall or violent impact detected!");
            updateDisplay("FALL DETECTED!", ST77XX_RED, 2);
            return true;
        }
        return false;
    }
}


void CatAlarm::updateDisplay(const char* message, uint16_t color, int font_size) {
    _carrier.display.fillScreen(ST77XX_BLACK);
    _carrier.display.setTextColor(color);
    _carrier.display.setTextSize(font_size); 
    _carrier.display.setCursor(20, 100);
    _carrier.display.print(message);
}

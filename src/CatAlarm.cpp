#include "CatAlarm.h" 
CatAlarm::CatAlarm(MKRIoTCarrier& carrier, bool& isSysTrigger ) : _carrier(carrier), _isTriggered(isSysTrigger), _fallDetector(true), _beepCounter(0),
    _lastBeepTime(0){}

void CatAlarm::begin() {
    _carrier.display.fillScreen(ST77XX_BLACK);
    _carrier.leds.clear(); // disable leds
    _carrier.leds.show();  // apply leds changes
}

void CatAlarm::trigger(bool active) {
    if (active) {
        if (!_isTriggered) { 
            _carrier.leds.fill(_carrier.leds.Color(255, 255, 0), 0, 5);
            _carrier.leds.show();             
            _isTriggered = true;
            _beepCounter = 0;               
            _lastBeepTime = millis();                    
        }
    } 
    else {
        _isTriggered = false; 
         _carrier.Buzzer.noSound(); // force buzzer stop 
        _carrier.leds.fill(_carrier.leds.Color(0, 55, 0), 0, 5);
        _carrier.leds.show();
    }
}

void CatAlarm::update() {
    if (!_isTriggered) return;

    static bool soundStatus = false; 
    long int currentTime = millis();

    if(!soundStatus && ( (currentTime - _lastBeepTime) >= random(200) + 100)) { 
        _carrier.Buzzer.sound(random(4000) + 870);
        soundStatus = true;
        _lastBeepTime = currentTime; 
    } 

    else if (soundStatus && ((currentTime - _lastBeepTime) >= random(400) + 100 )) { 
        _carrier.Buzzer.noSound();
        soundStatus = false;
        _beepCounter++;
        _lastBeepTime = currentTime; 
    }
    
    if (_beepCounter >= 7){
        trigger(false);
    }
    
}

bool CatAlarm::fallDetector(bool active) {
    _fallDetector = active;
    if (_fallDetector)
    { 
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
                updateDisplay("FALL DETECTED!", ST77XX_RED, 3);
                return true;
            }
            return false;
    }
}
}

void CatAlarm::updateDisplay(const char* message, uint16_t color, int font_size) {
    _carrier.display.fillScreen(ST77XX_BLACK); // clear 
    _carrier.display.setTextColor(color);
    _carrier.display.setTextSize(font_size); 
    _carrier.display.setCursor(20, 100);
    _carrier.display.print(message);
}

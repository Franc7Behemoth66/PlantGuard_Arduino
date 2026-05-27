#pragma once

#include <Arduino_MKRIoTCarrier.h>
#include <IMUClass.h>

class CatAlarm {

public:
    CatAlarm(MKRIoTCarrier& carrier, bool& isSysTrigger);
    void begin();
    void update();
    void trigger(bool active);
    bool fallDetector(bool active);
    void updateDisplay(const char* message, uint16_t color, int font_size);

private:
    MKRIoTCarrier& _carrier;
    bool  _fallDetector;
    bool &_isTriggered; // reference to a bool variable that determinate if the cat is present or noy, located in main.cpp
    int _beepCounter;
    long int _lastBeepTime; 
    long int _lastBlinkTime; // LED blink timer
};
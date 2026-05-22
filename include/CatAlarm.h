#pragma once

#include <Arduino_MKRIoTCarrier.h>
#include <IMUClass.h>

class CatAlarm {

public:
    CatAlarm(MKRIoTCarrier& carrier);
    void begin();
    void update();
    void trigger(bool active);
    bool fallDetector(bool active);
    void updateDisplay(const char* message, uint16_t color, int font_size);

private:

    MKRIoTCarrier& _carrier;
    bool  _fallDetector;
    bool _isTriggered;
    int _nextDelay;
    int _beepCounter;
    int _lastBeepTime;
};
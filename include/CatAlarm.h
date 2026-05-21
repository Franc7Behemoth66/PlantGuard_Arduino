#ifndef CAT_ALARM_H
#define CAT_ALARM_H

#include <Arduino_MKRIoTCarrier.h>
#include <IMUClass.h>

class CatAlarm {

public:
    CatAlarm(MKRIoTCarrier& carrier);
    void begin();
    void trigger(bool active);
    void fallDetector(bool active);
    void updateDisplay(const char* message, uint16_t color, int font_size);

private:

    MKRIoTCarrier& _carrier;
    bool  _fallDetector;
    bool _isTriggered;
};

#endif 
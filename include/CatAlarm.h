#ifndef CAT_ALARM_H
#define CAT_ALARM_H



#include <Arduino_MKRIoTCarrier.h>


class CatAlarm {

public:

    CatAlarm(MKRIoTCarrier& carrier);

    void begin();

    void trigger(bool active);

    void updateDisplay(const char* message, uint16_t color);

private:

    MKRIoTCarrier& _carrier;

  
    bool _isTriggered;
};

#endif 
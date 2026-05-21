#ifndef ROOM_ENVIRONMENT_H
#define ROOM_ENVIRONMENT_H

#include <Arduino_MKRIoTCarrier.h>

class RoomEnvironment {
    public:
        RoomEnvironment(MKRIoTCarrier& carrier);
        bool init();
        double getTemperature();
        double getHumidity();
        
    private:
        MKRIoTCarrier& _carrier; 
};

#endif 
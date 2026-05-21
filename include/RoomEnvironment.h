#ifndef ROOM_ENVIRONMENT_H
#define ROOM_ENVIRONMENT_H

#include <Arduino_MKRIoTCarrier.h>
#include <EnvClass.h>

class RoomEnvironment{


    MKRIoTCarrier _carrier;
    
    public:
        RoomEnvironment(MKRIoTCarrier& carrier);

        double getTemperature();
        double getHumidity();
        
    
};

#endif 
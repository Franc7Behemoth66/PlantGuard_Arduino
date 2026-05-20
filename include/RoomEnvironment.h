#ifndef ROOM_ENVIRONMENT_H
#define ROOM_ENVIRONMENT_H

#include <Arduino_MKRIoTCarrier.h>
#include <EnvClass.h>

class RoomEnvironment{

    public:
        RoomEnvironment(MKRIoTCarrier& carrier);

        double getTemperature();
        double getHumidity();
        
    private:

        MKRIoTCarrier _carrier;
        bool init();



};

#endif 
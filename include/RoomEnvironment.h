#pragma once

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
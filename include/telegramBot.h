#pragma once
// bot cmds
#define INFO_BOT "/help"
#define INIT_GUARD "/active_guard" 
#define STOP_GUARD "/stop_guard"
#define TEST_ALARM "/test"
#define REQ_DATA "/plant_health" // a bunch of data, such as temperature and Himidity

#define BOT_POLL_INTERVAL 1000 // check for bot cmds every 1 s

#include <WiFiNINA.h>
#include <ezTime.h> // obtain the current time zone
#include <UniversalTelegramBot.h>

// local libs
#include "secrets.h"
#include "CatAlarm.h"
#include "testHardware.h"

class telegramBot {

    public:
        telegramBot();
        void update();
        void begin(MKRIoTCarrier& carrier,bool& state, Timezone& myTimeZone);
        void sendMessage(const String& text);
        
    private:
        WiFiSSLClient _client;
        UniversalTelegramBot *_bot; //pointer to UniversalTelegramBot object
        MKRIoTCarrier* _carrier; // ptr to a MKRIoTCarrier object
        bool *_arduinoBoardState; // ptr to a bool variable tha that determinate the status of the sys, located in main.cpp
        Timezone *_tz; // pointer to the clock 
        void _handleMessage(telegramMessage& msg); 
        String  _avaibleCmds();
  
};
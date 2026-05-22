#pragma once
// bot cmds
#define INFO_BOT "/info"
#define INIT_GUARD "/active_guard" 
#define STOP_GUARD "/stop_guard"
#define TEST_ALARM "/test"
#define REQ_DATA "/plant_health" // a bunch of data, such as temperature and CO2 (soil detector??)

#include <WiFiNINA.h>
#include <UniversalTelegramBot.h>
#include <time.h>
// local libs
#include "secrets.h"
#include "RoomEnvironment.h"
#include "CatAlarm.h"
#include "testHardware.h"

class telegramBot {

    public:
        telegramBot();
        void update();
        void begin(MKRIoTCarrier& carrier,bool& state);
        void sendMessage(const String& text);
        bool isSystemActive();
        
    private:

        WiFiSSLClient _client;
        UniversalTelegramBot *_bot; //pointer to UniversalTelegramBot object
        RoomEnvironment * _env;
        bool *_arduinoBoardState;
        void _handleMessage(telegramMessage& msg); 
        String _formatTimestamp(String unixStr);
        String  _avaibleCmds();
  
};
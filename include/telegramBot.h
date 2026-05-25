#pragma once
// bot cmds
#define INFO_BOT "/help"
#define INIT_GUARD "/active_guard" 
#define STOP_GUARD "/stop_guard"
#define TEST_ALARM "/test"
#define REQ_DATA "/plant_health" // a bunch of data, such as temperature and Himidity

#define BOT_POLL_INTERVAL 1000 // check for bot cmds every 1 s

#include <WiFiNINA.h>
#include <UniversalTelegramBot.h>
#include <time.h>
// local libs
#include "secrets.h"
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
        MKRIoTCarrier* _carrier; // ptr to a MKRIoTCarrier object
        bool *_arduinoBoardState; // ptr to a bool variable that determinate the status of the sys, located in main.cpp
        void _handleMessage(telegramMessage& msg); 
        String _formatTimestamp(String unixStr);
        bool _isCest(time_t t); //return true for CEST (Summer time, UTC+2) and false for CET (Standard time, UTC+1).
        String  _avaibleCmds();
  
};
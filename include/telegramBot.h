#ifndef TELEGRAM_BOT_H
#define TELEGRAM_BOT_H

#define INIT_BOT "/mode_active " 
#define STOP_BOT "/mode_silent"
#define TEST_ALARM "/test"
#define REQ_DATA "/plant_health" // a bunch of data, such as temperature and CO2 (soil detector??)


#include <WiFiNINA.h>
#include <UniversalTelegramBot.h>
#include "secrets.h"
#include <time.h>


class telegramBot {


    public:
        telegramBot();
        void update();
        void begin();
        void sendMessage(const String& text);
        
    private:

        WiFiSSLClient _client;
        UniversalTelegramBot *_bot; //pointer to UniversalTelegramBot object
        void _handleMessage(telegramMessage& msg); 
        String _formatTimestamp(String unixStr);
  
};

#endif
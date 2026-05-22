#include "telegramBot.h"

telegramBot::telegramBot() : _bot(nullptr), _carrier(nullptr), _arduinoBoardState(nullptr)  {}

void telegramBot ::begin( MKRIoTCarrier& carrier, bool& state){ 
    _bot = new UniversalTelegramBot(BOT_TOKEN, _client);
    _carrier = &carrier;
    _arduinoBoardState = &state;
}

void telegramBot :: update() 
{
    int n = _bot->getUpdates(_bot->last_message_received + 1);
    for (int i = 0; i < n; i++) 
        _handleMessage(_bot->messages[i]);
    
}

void telegramBot::_handleMessage(telegramMessage& msg) {
    String text    = msg.text;
    String chat_id = msg.chat_id;
    String bot_reply;

    // security layer: only the owner of the bot can use it 
    if (chat_id != USER_ID) {
        Serial.println("[SECURITY] User " + chat_id+ " not authorized");
        _bot->sendMessage(chat_id,"You are not authorized to use this bot");
        return; 
    }

    String msgTime = _formatTimestamp(msg.date);
    Serial.println("[BOT] User authorized " +chat_id +", on date " +msgTime + ", sent " + msg.text ); 

    if(text == INIT_GUARD ){ 
        if(! *_arduinoBoardState ){
            * _arduinoBoardState = true;
            _bot->sendMessage(chat_id,"The Arduino system has been power on");
        }
        else
            _bot->sendMessage(chat_id,"The Arduino system was already power on");
    }

    else if(text == STOP_GUARD){
        if( *_arduinoBoardState ){
            * _arduinoBoardState = false;
            _bot->sendMessage(chat_id,"The Arduino system has been power off");
        }
        else
            _bot->sendMessage(chat_id,"The Arduino system was already power off");
    }

    else if(text == TEST_ALARM){
        Serial.println("[SYS]Test started");
        bot_reply = run_all_hardware_tests();
        _bot->sendMessage(chat_id,bot_reply);        
    }

    else if(text == REQ_DATA){
        double temp = _carrier->Env.readTemperature();
        double humidity = _carrier->Env.readHumidity();
        bot_reply = " Environmental data:\n Temperature: " + String(temp) + " °C\n Humidity: " + String(humidity) + " %";
        _bot->sendMessage(chat_id, bot_reply);
    }

    else if(text == INFO_BOT )
    {
        bot_reply = _avaibleCmds();
        _bot->sendMessage(chat_id, bot_reply);
    }
       
    else{
         bot_reply = "The cmd given does not exist" + _avaibleCmds();
        Serial.println("[BOT] ERROR: The text do not matches whit any of the avaible cmds");
        _bot->sendMessage(chat_id, bot_reply);

    }
}
String telegramBot::_avaibleCmds() {
    return "Available cmds:\n"
           "✤ " + String(INFO_BOT) + " → shows available cmds\n"
           "✤ " + String(INIT_GUARD) + " → activate guard\n"
           "✤ " + String(STOP_GUARD) + " → deactivate guard\n"
           "✤ " + String(TEST_ALARM) + " → test hardware\n"
           "✤ " + String(REQ_DATA) + " → plant health data\n";
}

bool telegramBot :: _isCest(time_t utc){
      struct tm* t = gmtime(&utc);
      int month = t->tm_mon + 1; // then january = 0, now january = 1 
           // Inside the April-September window CEST
        if(month > 3 && month < 10)
            return true;
        // Outside the March-October window: definitely Standard Time (CET)
        else if(month < 3 || month > 9)
            return false;
              // If the result is >= 25, we are in the last week of the month.
        int prevSunday = t->tm_mday- t->tm_wday; // n days of the month [1-31] - n days since last sunday [0-6]

        //it's CEST start at transition Sunday at 01:00 UTC (02:00 CET)
        if(month == 3){
            if(prevSunday >= 25){
                if(t->tm_wday == 0 )
                    return t->tm_hour >= 1; //true if it's true, otherwise false
                return true;
            }
            return false;
        }
      // If today is the transition Sunday, CEST ends at 01:00 UTC (03:00 CEST)
        if(month == 10){
            if(prevSunday >= 25){
                 if(t->tm_wday == 0 )
                    return t->tm_hour < 1;
                return false;
            }
            return true;
            }
}

String telegramBot:: _formatTimestamp(String unixStr) {
    time_t utc = (time_t)unixStr.toInt();
    utc += _isCest(utc) ? 7200 : 3600;
    struct tm* tm_info = gmtime(&utc);
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", tm_info);
    return String(buffer);
}

void telegramBot :: sendMessage(const String& text ){
    _bot->sendMessage(USER_ID,text);
}

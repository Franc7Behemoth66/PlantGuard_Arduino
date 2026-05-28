#include "telegramBot.h"

telegramBot::telegramBot() : _bot(nullptr), _carrier(nullptr), _arduinoBoardState(nullptr), _tz(nullptr)  {}

void telegramBot ::begin( MKRIoTCarrier& carrier, bool& state, Timezone& myTimeZone){ 
    _bot = new UniversalTelegramBot(BOT_TOKEN, _client);
    _carrier = &carrier;
    _arduinoBoardState = &state;
    _tz = &myTimeZone;

    _bot->getUpdates(_bot->last_message_received + 1); // discard the old messages
}

void telegramBot :: update() {
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
        return;}

    String *msgTime = &_tz->dateTime("l, d-M-Y H:i");
    Serial.println("[BOT] User authorized " +chat_id +", on date " +*msgTime + ", sent " + msg.text ); 

    if(text == INIT_GUARD ){ 
        if(! *_arduinoBoardState ){
            * _arduinoBoardState = true;
            bot_reply = "[ " +*msgTime + " ]" + "The Arduino system has been power on";
            sendMessage(bot_reply);
        }
        else
            sendMessage("The Arduino system was already power on");
    }

    else if(text == STOP_GUARD){
        if( *_arduinoBoardState ){
            * _arduinoBoardState = false;
            bot_reply = "[ " +*msgTime + " ]" + "The Arduino system has been power off" ;
            sendMessage(bot_reply);
        }
        else
            sendMessage("The Arduino system was already power off");
    }

    else if(text == TEST_ALARM){
        Serial.println("[SYS]Test started");
        bot_reply = run_all_hardware_tests();
        sendMessage(bot_reply);        
    }

    else if(text == REQ_DATA){
        double temp = _carrier->Env.readTemperature();
        double humidity = _carrier->Env.readHumidity();
        bot_reply = "[ Report of: " +*msgTime + " ]" + "\nEnvironmental data :\n temperature: " + String(temp) + " °C\n humidity: " + String(humidity) + " %";
        sendMessage(bot_reply);
    }

    else if(text == INFO_BOT ){
        bot_reply = _avaibleCmds();
        sendMessage(bot_reply);
    }

    else if(text.startsWith(SET_TIME_ZONE)){
        String timeZValue = text.substring(String(SET_TIME_ZONE).length());
        timeZValue.trim(); // remove spaces
        int offset = timeZValue.toInt();

        if(offset <= 14 && offset >= -12){ // -k become k || or k become -k, ezTime use a reverse logic 
            String posix_string = "UTC";

            if(offset <= 0)
                posix_string += "+" + String(abs(offset));
            else 
                posix_string += "-" + String(offset);

            _tz->setPosix(posix_string);
            msgTime = &_tz->dateTime("l, d-M-Y H:i");
            bot_reply = " Timezone updated successfully!\nCurrent local time:"  +*msgTime ;
            Serial.println("[BOT] Timezone changed to " + posix_string);

        } else 
            bot_reply = "Invalid offset. Please use a value between -12 and 14.";
        
        sendMessage(bot_reply);
    }
       
    else{
         bot_reply = "The cmd given does not exist:\n" + _avaibleCmds();
        Serial.println("[BOT] ERROR: The text do not matches whit any of the avaible cmds");
        sendMessage(bot_reply);

    }
}
String telegramBot::_avaibleCmds() {
    return "Available cmds:\n"
           "✤ " + String(INFO_BOT) + " → shows available cmds\n"
           "✤ " + String(INIT_GUARD) + " → activate guard\n"
           "✤ " + String(STOP_GUARD) + " → deactivate guard\n"
           "✤ " + String(TEST_ALARM) + " → test hardware\n"
           "✤ " + String(REQ_DATA) + " → plant health data\n"
           "✤ " + String(SET_TIME_ZONE) + " + [hours after/before UTC (es: `/time_zone 2`)] → set new time zone\n";
}

void telegramBot :: sendMessage(const String& text ){
    _bot->sendMessage(USER_ID,text);
}

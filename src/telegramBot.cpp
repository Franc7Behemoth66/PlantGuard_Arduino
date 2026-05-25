#include "telegramBot.h"

telegramBot::telegramBot() : _bot(nullptr), _carrier(nullptr), _arduinoBoardState(nullptr)  {}

void telegramBot ::begin( MKRIoTCarrier& carrier, bool& state, Timezone& myTimeZone){ 
    _bot = new UniversalTelegramBot(BOT_TOKEN, _client);
    _carrier = &carrier;
    _arduinoBoardState = &state;
    *_tz = myTimeZone;

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

    String msgTime = _tz->dateTime("l, d-M-Y H:i");
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
        bot_reply = "Report of : " +_tz->dateTime("l, d-M-Y H:i") + ".Environmental data :\n Temperature: " + String(temp) + " °C\n Humidity: " + String(humidity) + " %";
        _bot->sendMessage(chat_id, bot_reply);
    }

    else if(text == INFO_BOT ){
        bot_reply = _avaibleCmds();
        _bot->sendMessage(chat_id, bot_reply);
    }
       
    else{
         bot_reply = "The cmd given does not exist:\n" + _avaibleCmds();
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

void telegramBot :: sendMessage(const String& text ){
    _bot->sendMessage(USER_ID,text);
}

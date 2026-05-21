#include "telegramBot.h"

telegramBot::telegramBot() : _bot(nullptr), _env(nullptr) {}

void telegramBot ::begin( MKRIoTCarrier& carrier, bool& state){
    _bot = new UniversalTelegramBot(BOT_TOKEN, _client);
    _env = new RoomEnvironment(carrier);
    *_arduinoBoardState = state;
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

    String msgTime = _formatTimestamp(msg.date);
    Serial.println("[BOT] User " +chat_id +", on date " +msgTime + ", sent " + msg.text );

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
            * _arduinoBoardState = true;
            _bot->sendMessage(chat_id,"The Arduino system has been power off");
        }
        else
            _bot->sendMessage(chat_id,"The Arduino system was already power off");
    }

    else if(text == TEST_ALARM){
        bot_reply = "";
        
    }

    else if(text == REQ_DATA){

        double temp = _env->getTemperature();
        double humidity = _env->getTemperature();
        bot_reply = " Environmental data:\n Temperature: " + String(temp) + " °C\n Humidity: " + String(humidity) + " %";
        _bot->sendMessage(chat_id, bot_reply);

    }
    else{
        Serial.println("[BOT] ERROR: The text do not matches whit any of the avaible cmds, pls retype your cmd");
         bot_reply = "The text do not matches whit any of the avaible cmds, pls retype your cmd";
        _bot->sendMessage(chat_id, bot_reply);

    }

}

   


    String telegramBot:: _formatTimestamp(String unixStr) {
    time_t t = (time_t)unixStr.toInt();
    t += 2 * 3600; // UTC+2 ora italiana
    struct tm* tm_info = gmtime(&t);
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", tm_info);
    return String(buffer);
}

    

    

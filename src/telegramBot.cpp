#include "telegramBot.h"

telegramBot::telegramBot() : _bot(nullptr) {}

void telegramBot ::begin(){
    _bot = new UniversalTelegramBot(BOT_TOKEN, _client);
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

    Serial.println("[BOT] User " +chat_id +", on date " +msg.date + ", sent " + msg.text );


    if(text == INIT_BOT){
        
    }

    else if(text == STOP_BOT){
    }


    else if(text == TEST_ALARM){

    }

    else if(text == REQ_DATA){

    }
    else{
        Serial.println("[BOT] ERROR: The text do not matches whit any of the avaible cmds, pls retype your cmd");
        _bot->sendMessage(chat_id,"The text do not matches whit any of the avaible cmds, pls retype your cmd");

    }

    

    
}
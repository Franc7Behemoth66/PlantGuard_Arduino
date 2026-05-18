
#ifndef THING_PROPERTIES_H
#define THING_PROPERTIES_H

#include "secrets.h" // hidden .h/.hpp file which contains your wi-fi password, ssid, bot api and your personal ID associeted to the bot


const char SSID[]     =  SSID_WIFI;    
const char PASS[]     = PASSWORD_WIFI;
 


void onGattoPresenteChange();


bool gatto_presente;


#endif 
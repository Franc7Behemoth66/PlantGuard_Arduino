#include <Arduino.h>
#undef abs 
#include <Arduino_MKRIoTCarrier.h>
#include <WiFiNINA.h>

#include "secrets.h" // .h file that contains sensitive data, such as 
#include "telegramBot.h"
#include "CatAlarm.h"

MKRIoTCarrier carrier;
telegramBot bot;
CatAlarm alarm(carrier);

bool isArduinoActive = true;
int lastBotCheck = 0;

int wifiStatus = WL_IDLE_STATUS; 
const int pinPIR = A6; 

void setup() {
  Serial.begin(9600);

  /*--------------------------------------------/
              Wi-Fi configuration
  --------------------------------------------*/
  Serial.println("\n--- start WI-Fi configuration ---");
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("[SYS/HARD] Wi-Fi Chip not found on the board!");
    while (true); 
  }
  while (wifiStatus != WL_CONNECTED)
  {
    Serial.print("[SYS] Trying to connect to ");
    Serial.println(SSID_WIFI);
    wifiStatus = WiFi.begin(SSID_WIFI, PASSWORD_WIFI); // connection request
    Serial.println("\n Network status: ");
    Serial.println(wifiStatus);
    delay(5000); // wait 5 sec 
  }
  Serial.print("[SYS] WI-FI connected, MKR board IP address: ");
  Serial.println(WiFi.localIP());
    /*--------------------------------------------/
              bot and carrier initialization
  --------------------------------------------*/
  carrier.begin();     
  CARRIER_CASE = false; 

  bot.begin(carrier, isArduinoActive);
  pinMode(pinPIR, INPUT);
  delay(5000); 
}

void loop() {
  
  if(millis() - lastBotCheck >= BOT_POLL_INTERVAL  )  {
    bot.update();
    lastBotCheck = millis();
  }

  if(isArduinoActive){
    Serial.println("[SYS] PLANT GUARD SYSTEM ACTIVE");
    bool thereIsCat = digitalRead(pinPIR);
    if(thereIsCat){
          Serial.println("[SYS] Cat detected");
          alarm.updateDisplay("Cat detected\n/\\_/\\\n( o.o )\n > ^ < ", ST77XX_WHITE, 1);
          alarm.trigger(true);
    }
    else{
          Serial.println("[SYS] Looking for cats");
          alarm.updateDisplay("Looking for cats", ST77XX_WHITE, 1);
    }
    if(alarm.fallDetector(true)){
        bot.sendMessage("ALARM: the plant has fallen or it's been attacked");
        bot.sendMessage("IMPORTANT: Once the alarm has finished playing, the system will be deactivated. Please check the plant.");
        alarm.updateDisplay("Plant fallen/ under attack", ST77XX_RED,2 );
        alarm.trigger(true); 
        isArduinoActive = false;
    }
  }

  alarm.update(); // the alarm continue to play, no matter what cmds gives the user, until it reaches 7 beeps
  
  delay(10);
}





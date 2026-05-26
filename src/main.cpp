#include <Arduino.h>
#undef abs 
#include <Arduino_MKRIoTCarrier.h>
#include <WiFiNINA.h>
#include <ezTime.h>

#include "secrets.h" // .h file that contains sensitive data, such as 
#include "telegramBot.h"
#include "CatAlarm.h"

bool isArduinoActive = false;
bool catDetected = false;
unsigned long lastBotCheck = 0;
bool isScreenUpdated = false ; // in order to avoid a continue rewrite of the same string

MKRIoTCarrier carrier;
telegramBot bot;
CatAlarm alarm(carrier, catDetected);
Timezone timeZone;

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
              bot, clock and  carrier initialization
  --------------------------------------------*/
  CARRIER_CASE = false; 
  carrier.begin();
  waitForSync();// blocks until NTP sync is complete
  timeZone.setLocation("geoip"); // detects timezone from the board's public IP

  bot.begin(carrier, isArduinoActive, timeZone);
  pinMode(pinPIR, INPUT);
  delay(5000); 

}

void loop() {
  events(); // refresh the clock

  if(millis() - lastBotCheck >= BOT_POLL_INTERVAL  )  {
    bot.update();
    lastBotCheck = millis();
  }

  if(isArduinoActive){
    Serial.println("[SYS] PLANT GUARD SYSTEM ACTIVE");

    if(!catDetected){

      bool thereIsCat = digitalRead(pinPIR);
      if(thereIsCat){
          Serial.println("[SYS] Cat detected");
          alarm.updateDisplay("Cat detected\n /\\_/\\\n( o.o )\n > ^ < ", ST77XX_WHITE, 4);
          alarm.trigger(true);
          catDetected = true;
          isScreenUpdated = false;
      } 
      else if (!isScreenUpdated){
          Serial.println("[SYS] Looking for cats");
          alarm.updateDisplay("Looking for cats", ST77XX_WHITE, 4);
          isScreenUpdated = true;
      }
    }

    if(alarm.fallDetector(true)){
        bot.sendMessage("ALARM: the plant has fallen or it's been attacked");
        bot.sendMessage("IMPORTANT: Once the alarm has finished playing, the system will be deactivated. Please check the plant.");
        alarm.updateDisplay("Plant fallen / under attack", ST77XX_RED,4 );
        alarm.trigger(true); 
        isArduinoActive = false;
        isScreenUpdated = false;
    }

  }

  alarm.update(); // the alarm continue to play, no matter what cmds gives the user, until it reaches 7 beeps

  
  delay(10);
}





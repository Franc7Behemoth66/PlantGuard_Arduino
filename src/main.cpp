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

int wifiStatus = WL_IDLE_STATUS; 
const int pinPIR = A6; 

void setup() {
  Serial.begin(9600);

  /*--------------------------------------------/
              Wi-Fi configuration
  --------------------------------------------*/
  Serial.println("\n--- start WI-Fi configuration ---");
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println(" Wi-Fi Chip not found on the board!");
    while (true); 
  }
  while (wifiStatus != WL_CONNECTED)
  {
    Serial.print("Trying to connect to ");
    Serial.println(SSID_WIFI);
    wifiStatus = WiFi.begin(SSID_WIFI, PASSWORD_WIFI); // connection request
    Serial.println("\n Network status: ");
    Serial.println(wifiStatus);
    delay(5000); // wait 5 sec 
  }
  Serial.print("WI-FI connected, MKR board IP address: ");
  Serial.println(WiFi.localIP());

    /*--------------------------------------------/
              bot and carrier initialization
  --------------------------------------------*/
  carrier.begin();     
  bot.begin(carrier, isArduinoActive);
  
  CARRIER_CASE = false; 
  pinMode(pinPIR, INPUT);

  delay(5000); 
}

void loop() {
  
  bot.update();
  alarm.begin();
  if(isArduinoActive){

    bool thereIsCat = digitalRead(pinPIR);
      if(thereIsCat){
          Serial.println("[SYS] Cat detected");
          alarm.trigger(true);
          alarm.update();

          if(alarm.fallDetector(true)){
            bot.sendMessage("ALARM: the plant has fallen, the system in no longer active");
            alarm.updateDisplay("Plant fallen", ST77XX_RED,8 );
            isArduinoActive = false;
          }
      }

  }
 
  delay(10);
}




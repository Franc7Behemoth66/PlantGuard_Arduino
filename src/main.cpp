#include <Arduino.h>
#undef abs 
#include <Arduino_MKRIoTCarrier.h>
#include <WiFiNINA.h>

#include "secrets.h" // .h file that contains sensitive data, such as 
#include "telegramBot.h"

MKRIoTCarrier carrier;
telegramBot bot;

bool isArduinoActive = true;

int wifiStatus = WL_IDLE_STATUS; 
const int pinPIR = A6; 

bool lastStatus = true; 

void setup() {
  Serial.begin(9600);

  // WI-FI configuration
  Serial.println("\n--- AVVIO CONFIGURAZIONE WI-FI ---");
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("ERRORE CRITICO: Chip Wi-Fi non rilevato sulla scheda!");
    while (true); // Blocca il programma per evitare crash peggiori
  }
  while (wifiStatus != WL_CONNECTED)
  {
    Serial.print("Trying to connect to ");
    Serial.println(SSID_WIFI);
    wifiStatus = WiFi.begin(SSID_WIFI, PASSWORD_WIFI);
    Serial.println("\n Network status: ");
    Serial.println(wifiStatus);
    delay(5000); // wait 5 sec 
  }

  Serial.print("WI-FI connected, MKR board IP address: ");
  Serial.println(WiFi.localIP());
  // bot initialization
  carrier.begin();     
  bot.begin(carrier, isArduinoActive);
  
  CARRIER_CASE = false; 
  pinMode(pinPIR, INPUT);
  // NOTA TECNICA: I sensori PIR impiegano circa 10-20 secondi per stabilizzarsi
  // quando ricevono corrente. Questa pausa evita i falsi allarmi all'accensione.
  delay(5000); 
}

void loop() {
  
  bot.update();
  if(isArduinoActive){

  }
  else{

  }
 
  delay(100);
}



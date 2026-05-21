#include <Arduino.h>
#undef abs  // dopo Arduino.h
#include <Arduino_MKRIoTCarrier.h>
#include <WiFiNINA.h>

#include "secrets.h" // .h file that contains sensitive data, such as 
#include "telegramBot.h"

#include <random>

MKRIoTCarrier carrier;
telegramBot bot;




int wifiStatus = WL_IDLE_STATUS; 
const int pinPIR = A6; 

// TRUCCO LOGICO: Impostiamo lo stato precedente a 'true'. All'avvio, se la stanza 
// è vuota (false), la condizione (false != true) sarà vera e costringerà lo schermo
// ad aggiornarsi immediatamente mostrando "Monitoraggio..." e i LED verdi.
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
    Serial.print("Tentativo di connessione alla rete SSID ");
    Serial.println(SSID_WIFI);
    Serial.println("\n Stato rete: ");
    

    wifiStatus = WiFi.begin(SSID_WIFI, PASSWORD_WIFI);
    Serial.println(wifiStatus);
    delay(5000); 
  }
  Serial.print("Rete connessa, indirizzo IP della scheda MKR: ");
  Serial.println(WiFi.localIP());


  // bot initialization
  bot.begin();
    
  
  CARRIER_CASE = false; 
  carrier.begin();      
  pinMode(pinPIR, INPUT);

  // NOTA TECNICA: I sensori PIR impiegano circa 10-20 secondi per stabilizzarsi
  // quando ricevono corrente. Questa pausa evita i falsi allarmi all'accensione.
  delay(5000); 
}

void loop() {
  // Plettura digitale pura (HIGH / LOW), molto più stabile
  //bool currentStatus = (digitalRead(pinPIR) == HIGH);

  bot.update();
  
  delay(100);
}



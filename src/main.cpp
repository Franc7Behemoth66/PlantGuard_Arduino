#include <Arduino.h>
#include <Arduino_MKRIoTCarrier.h>
#include <random>

MKRIoTCarrier carrier;
const int pinPIR = A6; 

// TRUCCO LOGICO: Impostiamo lo stato precedente a 'true'. All'avvio, se la stanza 
// è vuota (false), la condizione (false != true) sarà vera e costringerà lo schermo
// ad aggiornarsi immediatamente mostrando "Monitoraggio..." e i LED verdi.
bool lastStatus = true; 

void setup() {
  Serial.begin(9600);
  

  CARRIER_CASE = false; 
  carrier.begin();      
  pinMode(pinPIR, INPUT);

  // Messaggio di calibrazione iniziale
  carrier.display.fillScreen(ST77XX_BLACK);
  carrier.display.setTextColor(ST77XX_WHITE);
  carrier.display.setTextSize(2);
  carrier.display.setCursor(20, 100);
  carrier.display.print("AVVIO IN CORSO...");
  
  // NOTA TECNICA: I sensori PIR impiegano circa 10-20 secondi per stabilizzarsi
  // quando ricevono corrente. Questa pausa evita i falsi allarmi all'accensione.
  delay(5000); 
}

void loop() {
  // Plettura digitale pura (HIGH / LOW), molto più stabile
  bool currentStatus = (digitalRead(pinPIR) == HIGH);

  // Esegue l'azione solo quando lo stato del sensore cambia effettivamente
  if (currentStatus != lastStatus) {
      carrier.display.fillScreen(ST77XX_BLACK);
      
      if (currentStatus) {
          // Stato: Gatto rilevato 
          carrier.display.setTextColor(ST77XX_RED);
          carrier.display.setTextSize(3);
          carrier.display.setCursor(30, 80);
          carrier.display.print("GATTO!");
          
          carrier.leds.fill(carrier.leds.Color(255, 255, 0), 0, 5);//led Gialli, gatto rilevato
          carrier.leds.show();
          
          Serial.println("BUS_DATA: [ALLARME] Gatto rilevato!"); 
      } 
      else 
      {
          // Stato: Standby e monitoraggio
          carrier.display.setTextColor(ST77XX_GREEN);
          carrier.display.setTextSize(2);
          carrier.display.setCursor(20, 100);
          carrier.display.print("Monitoraggio...");
          
          carrier.leds.fill(carrier.leds.Color(0, 55, 0), 0, 5);
          carrier.leds.show();

          Serial.println("BUS_DATA: [OK] Area pulita"); 
      }
      lastStatus = currentStatus;
  }
  
  delay(100);
}

#include "CatAlarm.h" 
CatAlarm::CatAlarm(MKRIoTCarrier& carrier) : _carrier(carrier), _isTriggered(false) {
    // Il corpo è vuoto perché l'inizializzazione è già avvenuta nella lista sopra.
}


void CatAlarm::begin() {
    // _carrier è l'oggetto che rappresenta l'hardware. 
    
    _carrier.display.fillScreen(ST77XX_BLACK);
}

/* * METODO trigger(bool active)
 * Gestisce l'allarme visivo (LED) e sonoro (Buzzer).
 */
void CatAlarm::trigger(bool active) {
    if (active) {
        // .Color(R, G, B) genera un valore a 16 bit per i LED RGB.
        // fill(colore, inizio, fine): accende i 5 LED del carrier di rosso.
        _carrier.leds.fill(_carrier.leds.Color(255, 0, 0), 0, 5);
        _carrier.leds.show(); // Invia il comando ai LED (
        
        // Buzzer.beep(frequenza in Hz, durata in ms)
        _carrier.Buzzer.beep(9000, 100);
        
    } else {
        // Se non attivo, imposta i LED su un verde tenue (0, 55, 0).
        _carrier.leds.fill(_carrier.leds.Color(0, 55, 0), 0, 5);
        _carrier.leds.show();
    }
}

/* * METODO updateDisplay(const char* message, uint16_t color)
 * * const char* message: Passaggio di una stringa come PUNTATORE a caratteri costante.
 */
void CatAlarm::updateDisplay(const char* message, uint16_t color) {
    // Pulisce lo schermo prima di scrivere il nuovo messaggio.
    _carrier.display.fillScreen(ST77XX_BLACK);
    
    // Configurazione del testo
    _carrier.display.setTextColor(color);
    _carrier.display.setTextSize(2); // Dimensione del font
    
    // Imposta le coordinate X, Y sullo schermo (240x240 pixel tipicamente).
    _carrier.display.setCursor(20, 100);
    
    // Stampa il messaggio. 
    _carrier.display.print(message);
}
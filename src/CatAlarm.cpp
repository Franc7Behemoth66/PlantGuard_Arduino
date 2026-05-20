
#include "CatAlarm.h" 
CatAlarm::CatAlarm(MKRIoTCarrier& carrier) : _carrier(carrier), _isTriggered(false) {}


void CatAlarm::begin() {
    
    _carrier.display.fillScreen(ST77XX_BLACK);
    _carrier.leds.clear();

}


void CatAlarm::trigger(bool active) {
    if (active) {
        // .Color(R, G, B) genera un valore a 16 bit per i LED RGB.
        // fill(colore, inizio, fine): accende i 5 LED del carrier di giallo.
        _carrier.leds.fill(_carrier.leds.Color(255, 255, 0), 0, 5);
        _carrier.leds.show(); // Invia il comando ai LED (
        
        for (int i = 0; i < 17; i++){
            double freq_note = random(1000) + 4000;
            double period_note = random(700) + 130;
            double delay_time = random(550) + 10;

             // Buzzer.beep(frequenza in Hz, durata in ms)
            _carrier.Buzzer.beep(freq_note, period_note);
            delay(delay_time);
        }

       
    } 
    else 
    {
        // Se non attivo, imposta i LED su un verde tenue (0, 55, 0).
        _carrier.leds.fill(_carrier.leds.Color(0, 55, 0), 0, 5);
        _carrier.leds.show();

    }
}

void CatAlarm::updateDisplay(const char* message, uint16_t color, int font_size) {
    // Pulisce lo schermo prima di scrivere il nuovo messaggio.
    _carrier.display.fillScreen(ST77XX_BLACK);
    // Configurazione del testo
    _carrier.display.setTextColor(color);
    _carrier.display.setTextSize(font_size); // Dimensione del font
    // Imposta le coordinate X, Y sullo schermo (240x240 pixel tipicamente).
    _carrier.display.setCursor(20, 100);
    // Stampa il messaggio. 
    _carrier.display.print(message);
}

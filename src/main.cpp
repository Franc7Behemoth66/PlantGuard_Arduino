#include <Arduino.h>
#undef abs
// Undefine the legacy Arduino 'abs' macro to prevent namespace collisions.
// The default Arduino macro (#define abs(x)) destroys the syntax of standard 
// C++ libraries that rely on the type-safe std::abs() function.
#define EZTIME_NO_EEPROM // it shout down the EEPROM, becusase this model of arduino doesn't have one
#include <ezTime.h>
#include <Arduino_MKRIoTCarrier.h>
#include <WiFiNINA.h>

#include "secrets.h"
#include "telegramBot.h"
#include "CatAlarm.h"

bool isArduinoActive = false;
bool catDetected = false;
unsigned long lastBotCheck = 0;
bool isScreenUpdated = false;

Timezone timeZone;
MKRIoTCarrier carrier;
telegramBot bot;
CatAlarm alarm(carrier, catDetected);

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
    while (wifiStatus != WL_CONNECTED) {
        Serial.print("[SYS] Trying to connect to ");
        Serial.println(SSID_WIFI);
        wifiStatus = WiFi.begin(SSID_WIFI, PASSWORD_WIFI);
        Serial.println("\n Network status: ");
        Serial.println(wifiStatus);
        delay(5000);
    }
    Serial.print("[SYS] WI-FI connected, MKR board IP address: ");
    Serial.println(WiFi.localIP());
    /*--------------------------------------------/
              bot, clock and carrier initialization
    --------------------------------------------*/

    CARRIER_CASE = false;
    alarm.begin();
    carrier.begin();
    waitForSync();
    // if timeZone.setLocation("geoip") fails (due to a server error), the sys statically set the time on Italy's time zone
    if (!timeZone.setLocation("geoip")) {
        Serial.println("[TZ] geoip failed, using Italy fallback");
        timeZone.setPosix("CET-1CEST,M3.5.0,M10.5.0/3");
    }
    else
        Serial.println("[TZ] geoip sucesfull, time zone: " +timeZone.getTimezoneName());
    
    Serial.println("[TZ] Active timezone: " + timeZone.getPosix());

    bot.begin(carrier, isArduinoActive, timeZone);
    pinMode(pinPIR, INPUT);
    delay(5000);
}

void loop() {
    events();

    if (millis() - lastBotCheck >= BOT_POLL_INTERVAL) {
        bot.update();
        lastBotCheck = millis();
    }

    if (isArduinoActive) {
        Serial.println("[SYS] PLANT GUARD SYSTEM ACTIVE");

        if (!catDetected) {
            bool thereIsCat = digitalRead(pinPIR);
            if (thereIsCat) {
                Serial.println("[SYS] Cat detected");
                alarm.updateDisplay("Cat detected\n /\\_/\\\n( o.o )\n > ^ < ", ST77XX_WHITE, 4);
                alarm.trigger(true);
                catDetected = true;
                isScreenUpdated = false;
            } else if (!isScreenUpdated) {
                Serial.println("[SYS] Looking for cats");
                alarm.updateDisplay("Looking for cats", ST77XX_WHITE, 4);
                isScreenUpdated = true;
            }
        }

        if (alarm.fallDetector(true)) {
            bot.sendMessage("ALARM: the plant has fallen or it's been attacked");
            bot.sendMessage("IMPORTANT: Once the alarm has finished playing, the system will be deactivated. Please check the plant.");
            alarm.updateDisplay("Plant fallen / under attack", ST77XX_RED, 4);
            alarm.trigger(true);
            isArduinoActive = false;
            isScreenUpdated = false;
        }
    }

    alarm.update();
    delay(10);
}
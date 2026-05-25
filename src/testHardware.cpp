#include "testHardware.h"

extern MKRIoTCarrier carrier; // MKRIoTCarrier object defined in main.cpp

String test_display() {
    carrier.display.fillScreen(ST77XX_BLUE);
    carrier.display.setTextColor(ST77XX_WHITE);
    carrier.display.setTextSize(7);
    carrier.display.setCursor(120, 120);
    carrier.display.print("TEST DISP...");
    delay(300);
    carrier.display.fillScreen(ST77XX_BLACK);
    return "DISP OK\n";
}// not atomic fn

String test_temperature() {
    double t = carrier.Env.readTemperature();
    double h = carrier.Env.readHumidity();
    if (t > -20.0 && t < 80.0) {
        return "ENV: OK\n" ;
    }
    return "ENV: ERROR\n";
} // atomic fn

String test_gyroscope() {
    float x, y, z;
    if (carrier.IMUmodule.gyroscopeAvailable()) {
        carrier.IMUmodule.readGyroscope(x, y, z);
        return "IMU: OK \n";
    }
    return "IMU: OFFLINE/STUCK\n";
} // atomic fn

String test_leds() {
    uint32_t white = carrier.leds.Color(50, 50, 50); 
    carrier.leds.fill(white, 0, 5); 
    carrier.leds.show();
    delay(250);
    carrier.leds.fill(0, 0, 5); 
    carrier.leds.show();
    return "LED: TEST OK\n";
}// not atomic fn

String test_buzzer() {
    carrier.Buzzer.beep(2000, 50);
    carrier.Buzzer.beep(2000, 50);
    return "BUZZ: TEST OK\n";
}// not atomic fn


String run_all_hardware_tests() {
    String log = "--- HARDWARE TEST ---\n";
    log += test_display();
    log += test_temperature();
    log += test_gyroscope();
    log += test_leds();
    log += test_buzzer();    
    return log;
} 
//Some functions are not atomic by design: since bot input is checked every 1s,
// and the total delay of all test functions is approximately 650ms, no messages
// will be missed during the test execution.
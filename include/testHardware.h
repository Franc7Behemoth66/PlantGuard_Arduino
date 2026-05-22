#ifndef TEST_HARDWARE_H
#define TEST_HARDWARE_H
#include <Arduino.h>
#include <Arduino_MKRIoTCarrier.h>

String test_display();
String test_temperature();
String test_gyroscope();
String test_leds();
String test_buzzer();
String run_all_hardware_tests();

#endif
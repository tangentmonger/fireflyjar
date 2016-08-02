#ifndef FIREFLY_STRUCTS
#define FIREFLY_STRUCTS
#include <Arduino.h>

struct pin_port {
    byte portB; //digital pins 8-13
    byte portC; //analog pins
    byte portD; //digital pins 0-7
};

struct led_wiring {
    pin_port from;
    pin_port to;
};

struct request {
    led_wiring led1_wiring;
    int led1_brightness;
    led_wiring led2_wiring;
    int led2_brightness;
};

const led_wiring NULL_LED = {{0,0,0}, {0,0,0}};

#endif

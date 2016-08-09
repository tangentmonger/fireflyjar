#ifndef FIREFLY_STRUCTS
#define FIREFLY_STRUCTS
#include <Arduino.h>

struct pin_port {
    // Represents the three registers that control the I/O ports
    byte portB; //digital pins 8-13
    byte portC; //analog pins
    byte portD; //digital pins 0-7
};

struct led_wiring {
    // represents the pin connections for one LED
    pin_port from;
    pin_port to;
};

struct request {
    // a request for two LEDs to be lit, each with brightness as a percentage.
    // Typically used for adjacent LEDs to create the illusion that a single "point"
    // along the firefly has been lit.
    led_wiring led1_wiring;
    int led1_brightness;
    led_wiring led2_wiring;
    int led2_brightness;
};

const led_wiring NULL_LED = {{0,0,0}, {0,0,0}}; // corresponds to "no LED"

#endif

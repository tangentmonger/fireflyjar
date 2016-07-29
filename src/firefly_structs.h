// PORTS:
//  B (digital pin 8 to 13)
//  C (analog input pins)
//  D (digital pins 0 to 7)

struct pin_port {
    byte portB;
    byte portC;
    byte portD;
};

pin_port pin_2  = {0,0,0b00000100};
pin_port pin_3  = {0,0,0b00001000};
pin_port pin_4  = {0,0,0b00010000};
pin_port pin_5  = {0,0,0b00100000};
pin_port pin_6  = {0,0,0b01000000};
pin_port pin_7  = {0,0,0b10000000};
pin_port pin_8  = {0b00000001,0,0};
pin_port pin_9  = {0b00000010,0,0};
pin_port pin_10 = {0b00000100,0,0};
pin_port pin_11 = {0b00001000,0,0};
pin_port pin_12 = {0b00010000,0,0};
pin_port pin_13 = {0b00100000,0,0};
pin_port pin_A0 = {0,0b00000001,0};
pin_port pin_A1 = {0,0b00000010,0};
pin_port pin_A2 = {0,0b00000100,0};
pin_port pin_A3 = {0,0b00001000,0};
pin_port pin_A4 = {0,0b00010000,0};
pin_port pin_A5 = {0,0b00100000,0};

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

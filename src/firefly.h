#ifndef FIREFLY
#define FIREFLY
#include "firefly_structs.h"

#define LED_COUNT 12

class Firefly {
/*
Represents one hardware firefly with LED_COUNT LEDs.
*/
    public:
        Firefly(pin_port blue_wire, pin_port green_wire, pin_port yellow_wire, pin_port red_wire);
    
        void update();
        void begin_song();
        request get_led_request();

    private:
        bool _lit;
        int _position; //percentage
        int _brightness; //percentage
        led_wiring _led_wirings[LED_COUNT];
        request _led_request;
        int _convert_brightness(int brightness);
};
#endif


#ifndef FIREFLY
#define FIREFLY
#include "firefly_structs.h"

#define LED_COUNT 12
#define STANDARD_MAX_TIREDNESS 200 //in cycles of 5ms each
#define FRIGHTENED_MAX_TIREDNESS 200 //in cycles of 5ms each
#define MAX_HESITATION 30 // in cycles of 5ms each

class Firefly {
/*
Represents one hardware firefly with LED_COUNT LEDs.
*/
    public:
        // Constructor. Supply the pins connected to each of the four wires.
        Firefly(pin_port blue_wire, pin_port green_wire, pin_port yellow_wire, pin_port red_wire);

        // Update regularly to allow firefly to progress songs, etc
        void update();

        // Begin a song, if the firefly is able to
        void begin_song();

        // Frighten firefly (begin song even if tired)
        void frighten();

        // Getter for requested LED state
        request get_led_request();

    private:
        int _position; // lit "point", from the bottom (0) to the top (100)
        int _brightness; // brightness of lit "point" as a percentage
        int _tiredness; // singing makes the firefly tired and unable to sing again for a while
        led_wiring _led_wirings[LED_COUNT]; // pin connections for each LED
        request _led_request; // current requested LED state
        int _convert_brightness(int brightness); // S-curve brightness correction
        int _state; // current state of firefly
        int _hesitation; // short random delay before flying away
};
#endif


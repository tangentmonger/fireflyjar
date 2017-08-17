#include "firefly.h"
#include <math.h>

// State machine. Start in a ready state. Firefly becomes tired after a "song"
// (i.e. after lighting up) and won't sing again until rested, unless
// frightened by a tap on the jar. There is a brief hesitation before becoming
// frightened.Firefly becomes exhausted after being frightened, and won't sing
// or react to taps again until rested for a longer period. Return to ready
// state after resting.

#define STATE_READY 0
#define STATE_SINGING 1
#define STATE_TIRED 2
#define STATE_FRIGHTENED 3
#define STATE_EXHAUSTED 4
#define STATE_HESITATING 5

Firefly::Firefly(pin_port blue_wire, pin_port green_wire, pin_port yellow_wire, pin_port red_wire)
{
    // Represents one firefly with 12 LEDs. Controls its song and builds requests for
    // LEDs to be lit.
    _position = 0;
    _brightness = 0;
    _led_request = {NULL_LED, 0, NULL_LED ,0};
    _state = STATE_READY;
   
    // Wirings match v4 PCB
    //             LED +ve.......... -ve  
    // Top (head)
    _led_wirings[0] = {red_wire,     blue_wire};
    _led_wirings[1] = {blue_wire,    red_wire};

    _led_wirings[2] = {red_wire,     green_wire};
    _led_wirings[3] = {green_wire,   red_wire};
    
    _led_wirings[4] = {red_wire,     yellow_wire};
    _led_wirings[5] = {yellow_wire,  red_wire};
    
    _led_wirings[6] = {green_wire,   yellow_wire};
    _led_wirings[7] = {yellow_wire,  green_wire};
    
    _led_wirings[8] = {green_wire,   blue_wire};
    _led_wirings[9] = {blue_wire,    green_wire};
    
    _led_wirings[10] = {yellow_wire, blue_wire};
    _led_wirings[11] = {blue_wire,   yellow_wire};
    // Bottom (tail)
}

void Firefly::update() {
    // Called regularly to allow firefly to update its current LED pattern.
    if (_state == STATE_SINGING || _state == STATE_FRIGHTENED) {
        // Firefly is lit, so update position and brightness
        _position = (_position + 1);
        if (_position < 100) {
            if(_position < 50) {
                // First half fades in to full brightness...
                _brightness = _position * 2;
            } else {
                // ... second half fades out again
                _brightness = (100 - _position) * 2;
            }

            //invert, so that 0 is the bottom
            float position = 100 - _position;

            // adjust brightness
            //float brightness = _convert_brightness(_brightness);
            float brightness = _brightness;

            // 1) which LEDs do we light?
            float virtual_led = float(LED_COUNT-1) * position / 100.0;
            int lower_led = int(floor(virtual_led));
            int upper_led = int(ceil(virtual_led));

            // 2) in what proportions (as brightness percentage)?
            //assumtion: brightness is additive
            float proportion = virtual_led - float(lower_led);
            int lower_led_brightness = int(brightness * (1-proportion));
            int upper_led_brightness = int(brightness * proportion);

            // 3) ok, request that
            _led_request = {_led_wirings[lower_led], lower_led_brightness, _led_wirings[upper_led], upper_led_brightness};
        }
        else
        {
            // end the song, leaving firefly in a "tired" state so it won't
            // sing again for a while
            _led_request = {NULL_LED, 0, NULL_LED ,0};
            if (_state == STATE_SINGING) {
                _state = STATE_TIRED;
            } else {
                _state = STATE_EXHAUSTED;
            }
        }

    }
    else if (_state == STATE_TIRED || _state == STATE_EXHAUSTED)
    {
        // firefly is in a "tired" state, so reduce tiredness
        _tiredness--; 
        if (_tiredness == 0){
            _state = STATE_READY;
        }
    }
    else if (_state == STATE_HESITATING)
    {
        _hesitation--;
        if (_hesitation == 0){
            _position = 0;
            _brightness = 0;
            _led_request = {NULL_LED, 0, NULL_LED ,0};
            _tiredness = FRIGHTENED_MAX_TIREDNESS;
            _state = STATE_FRIGHTENED;
        }
    }
}

void Firefly::begin_song() {
    // if firefly is off and rested, begin a song
    if (_state == STATE_READY) {
        _position = 0;
        _brightness = 0;
        _led_request = {NULL_LED, 0, NULL_LED ,0};
        _tiredness = STANDARD_MAX_TIREDNESS;
        _state = STATE_SINGING;
    }
}

void Firefly::frighten() {
    // if firefly is off, begin a song (even if tired)
    if (_state == STATE_READY || _state == STATE_TIRED) {
        _hesitation = random(MAX_HESITATION) + 1; // never zero
        _state = STATE_HESITATING;
    }    
}

request Firefly::get_led_request() {
    // getter
    return _led_request;
}

int Firefly::_convert_brightness(int brightness)
{
    // Convert brightness on an S-curve
    // https://en.wikipedia.org/wiki/Sigmoid_function
    
    // Sigmoid function: S(t) = 1/(1 + e^-t)
    // Domain: -6ish to 6ish
    // Range: 0 to 1
    float e = 2.718;
    float stretch = 2.0; //how much of the curve to use. Increase for more contrast
    float t = (brightness - 50) / 50.0 * (e * stretch);
    float s = 1.0 / (1.0 + pow(e, -t));
    return int(s * 100.0);
}

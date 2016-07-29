#include <math.h>
#include "firefly_structs.h"

#define LED_COUNT 12
#define ledPin 13

class firefly {

    public:
        firefly(pin_port blue_wire, pin_port green_wire, pin_port yellow_wire, pin_port red_wire)
        {
            _lit = false;
            _position = 0;
            _brightness = 0;
            _led_request = {NULL_LED, 0, NULL_LED ,0};

            _ledarray = {
                {red_wire, green_wire},
                {green_wire, red_wire},
                {green_wire, blue_wire},
                {green_wire, yellow_wire},
                {yellow_wire, green_wire},
                {yellow_wire, red_wire},
                {yellow_wire, blue_wire},
                {blue_wire, yellow_wire},
                {blue_wire, green_wire},
                {blue_wire, red_wire},
                {red_wire, blue_wire},
                {red_wire, yellow_wire},
            };        
        }

        void update() {
            if (_lit) {
                //_position = 50; //(_position + 1) % 100;
                //_brightness = 100; //= (_brightness - 1) % 100;
                _position = (_position + 1);
                if (_position < 100) {
                    //_brightness = (_brightness + 1) % 100;

                    if(_position < 50) {
                        _brightness = _position * 2;
                    } else {
                        _brightness = (100 - _position) * 2;
                    }
                
                    //invert, so that 0 is the bottom
                    float position = 100 - _position;
                    //float position = _position;

                    // adjust brightness
                    float brightness = sCurvedBrightness(_brightness);
                    //float brightness = _brightness;

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
                    _led_request = {_ledarray[lower_led], lower_led_brightness, _ledarray[upper_led], upper_led_brightness};
                }
                else
                {
                    // song ends
                    _lit = false;
                }

            }
        }

        void begin_song() {
            if (_lit == false) {
                _lit = true;
                _position = 0;
                _brightness = 0;
                _led_request = {NULL_LED, 0, NULL_LED ,0};
            }
        }

        request get_led_request() {
            return _led_request;
        }

    private:
        bool _lit;
        int _position; //percentage
        int _brightness; //percentage
        led_wiring _ledarray[LED_COUNT];
        request _led_request;
};


void turnOn( led_wiring led ) {
    DDRB = DDRB | led.from.portB | led.to.portB;
    PORTB = PORTB | led.from.portB;
    DDRC = DDRC | led.from.portC | led.to.portC;
    PORTC = PORTC | led.from.portC;
    DDRD = DDRD | led.from.portD | led.to.portD;
    PORTD = PORTD | led.from.portD;
}

void turnOff() {
    DDRB = 0;
    PORTB = 0;
    DDRC = 0;
    PORTC = 0;
    DDRD = 0;
    PORTD = 0;
}

#define NUM_FIREFLIES 2
firefly fireflies[NUM_FIREFLIES] = {firefly(pin_2, pin_3, pin_4, pin_5),
                                    firefly(pin_6, pin_7, pin_8, pin_9)};

void setup() {
    turnOff();
    pinMode(ledPin, OUTPUT);
    // initialize timer1 
    noInterrupts();           // disable all interrupts
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1  = 0; //actual timer value

    OCR1A = 6; //625;            // compare match register 16MHz/256/100Hz
    TCCR1B |= (1 << WGM12);   // CTC mode (clear timer when it matches)
    TCCR1B |= (1 << CS12);    // 256 prescaler 
    TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
    interrupts();             // enable all interrupts
}
int sCurvedBrightness(int brightness)
{
    // Convert brightness on an S-curve
    // https://en.wikipedia.org/wiki/Sigmoid_function
    
    // Sigmoid function: S(t) = 1/(1 + e^-t)
    // Domain: -6ish to 6ish
    // Range: 0 to 1
    float e = 2.718;
    float stretch = 2.5; //how much of the curve to use. Increase for more contrast
    float t = (brightness - 50) / 50.0 * (e*stretch);
    float s = 1.0 / (1.0 + pow(e,-t));
    return int(s * 100.0);
}

request led_requests[NUM_FIREFLIES];


void loop() {
    int chance = random(200);
    if (chance == 0) {
        int firefly = random(NUM_FIREFLIES);
        fireflies[firefly].begin_song();
    }

    // keep count of lit bigs
    // if >2 are lit, randomly choose whether to light a random firefly
    
    // tell each firefly to update song
    // collect requests from fireflies

    for(int i=0; i<NUM_FIREFLIES; i++) {
        fireflies[i].update();
        led_requests[i] = fireflies[i].get_led_request();
    }

    
    // wait
    delay(5); //milliseconds. loads of time to service ISRs probably

}


volatile int duty_cycle = 0;

ISR(TIMER1_COMPA_vect)          // timer compare interrupt service routine
{
    duty_cycle = (duty_cycle + 1) % 100;

    turnOff();

    for(int i=0; i<NUM_FIREFLIES; i++) {
        request r = led_requests[i];
        if (duty_cycle < r.led1_brightness) {
            turnOn(r.led1_wiring);
            
        }
        else if (duty_cycle < r.led2_brightness + r.led1_brightness) {
            turnOn(r.led2_wiring);
        }
    }

    
 //digitalWrite(ledPin, digitalRead(ledPin) ^ 1);   // toggle LED pin
}

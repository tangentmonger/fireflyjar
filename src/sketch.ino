#include <math.h>
#include "firefly_structs.h"
#include "firefly.h"

#define ledPin 13

pin_port pin_0  = {0,0,0b00000001}; //tx
pin_port pin_1  = {0,0,0b00000010}; //rx
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
// A6 and A7 cannot be used as digital outputs
// pin_port pin_A6 = {0,0b01000000,0};
// pin_port pin_A7 = {0,0b10000000,0};

#define NUM_FIREFLIES 5

//                                          blue, green, yellow, red 
Firefly fireflies[NUM_FIREFLIES] = {Firefly(pin_0, pin_1, pin_2, pin_3),
                                    Firefly(pin_4, pin_5, pin_6, pin_7),
                                    Firefly(pin_8, pin_9, pin_10, pin_11),
                                    Firefly(pin_12, pin_13, pin_A0, pin_A1),
                                    Firefly(pin_A2, pin_A3, pin_A4, pin_A5)};


void setup() {
    turn_off();
    //pinMode(ledPin, OUTPUT);
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

request led_requests[NUM_FIREFLIES];

void loop() {
    int chance = random(200);
    if (chance == 0) {
        int selected_firefly = random(NUM_FIREFLIES);
        fireflies[selected_firefly].begin_song();
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

    turn_off();

    for(int i=0; i<NUM_FIREFLIES; i++) {
        request r = led_requests[i];
        if (duty_cycle < r.led1_brightness) {
            turn_on(r.led1_wiring);
            
        }
        else if (duty_cycle < r.led2_brightness + r.led1_brightness) {
            turn_on(r.led2_wiring);
        }
    }
    //digitalWrite(ledPin, digitalRead(ledPin) ^ 1);   // toggle LED pin
}

void turn_on( led_wiring led ) {
    DDRB = DDRB | led.from.portB | led.to.portB;
    PORTB = PORTB | led.from.portB;
    DDRC = DDRC | led.from.portC | led.to.portC;
    PORTC = PORTC | led.from.portC;
    DDRD = DDRD | led.from.portD | led.to.portD;
    PORTD = PORTD | led.from.portD;
}

void turn_off() {
    DDRB = 0;
    PORTB = 0;
    DDRC = 0;
    PORTC = 0;
    DDRD = 0;
    PORTD = 0;
}

#include <math.h>
#include "firefly_structs.h"
#include "firefly.h"

// Note: rx/tx hardware and charlieplexing are incompatible!
// Disconnect one before using the other.
pin_port pin_rx  = {0,0,0b00000001}; //Pro Mini: "RX1", Nano: "RX0"
pin_port pin_tx  = {0,0,0b00000010}; //Pro Mini: "TX0", Nano: "TX1"
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

int knock_sensor = A6; // piezo sensor with large resistor in parallel
int KNOCK_THRESHOLD = 100;

#define NUM_FIREFLIES 5
// #define TEST_MODE

//                                          blue, green, yellow, red 
Firefly fireflies[NUM_FIREFLIES] = {Firefly(pin_tx, pin_rx, pin_2, pin_3),
                                    Firefly(pin_4, pin_5, pin_6, pin_7),
                                    Firefly(pin_8, pin_9, pin_10, pin_11),
                                    Firefly(pin_12, pin_13, pin_A0, pin_A1),
                                    Firefly(pin_A2, pin_A3, pin_A4, pin_A5)};


void setup() {
    turn_off();
    
    // initialize timer1, used to run PWM on firefly LEDs
    noInterrupts();           // disable all interrupts
    TCCR1A = 0; // timer modes off etc
    TCCR1B = 0; // timer stopped
    TCNT1  = 0; // reset timer value to 0

    // timer interrupts at roughly 10kHz
    // speed: 16MHz (atmega328) / 256 (prescaler) / 6 (counter match) = 10416Hz
    OCR1A = 0x0006;           // value to match
    TCCR1B |= (1 << WGM12);   // CTC mode (clear timer when it matches)
    TCCR1B |= (1 << CS12);    // set prescaler to 256
    TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
    interrupts();             // enable all interrupts

    //Serial.begin(9600);
}

request led_requests[NUM_FIREFLIES];

void loop() {

#ifndef TEST_MODE

    byte knock_value = analogRead(knock_sensor);
    //Serial.println(knock_value);
    if (knock_value > KNOCK_THRESHOLD) {
        // detected a knock on the jar which frightens all fireflies
        // into flying at once
        for(int i=0; i<NUM_FIREFLIES; i++) {
            fireflies[i].frighten();
        }
    }
    else {
        int chance = random(200);
        if (chance == 0) {
            // attempt to start a song on a random firefly
            int selected_firefly = random(NUM_FIREFLIES);
            fireflies[selected_firefly].begin_song();
        }
    }

    // collect LED 'position' requests for all fireflies
    for(int i=0; i<NUM_FIREFLIES; i++) {
        fireflies[i].update();
        led_requests[i] = fireflies[i].get_led_request();
    }

    delay(5); //milliseconds. Service ISRs (for PWM) until next LED 'position' update.


#else

    // for testing, just keep invoking firefly 1 on pins 4-7
    fireflies[1].frighten();  // nothing will happen if it's already singing

    // collect LED 'position' requests for all fireflies
    fireflies[1].update();
    led_requests[1] = fireflies[1].get_led_request();

    delay(30); //milliseconds. Service ISRs (for PWM) until next LED 'position' update.

#endif

}

volatile int duty_cycle = 0; // keep track of where we are within PWM cycles

ISR(TIMER1_COMPA_vect)
{
    // Update the state of the LEDs within the PWM cycle, according to 
    // current LED requests.
    duty_cycle = (duty_cycle + 1) % 100;

    turn_off();

    for(int i=0; i<NUM_FIREFLIES; i++) {
        request r = led_requests[i];
        if (duty_cycle < r.led1_brightness) {
            turn_on(r.led1_wiring);
            
        }
        // assumption: no firefly has requested LEDs lit with a total 
        // brightness > 100. If they have, the second LED won't be as
        // bright as requested.
        else if (duty_cycle < r.led2_brightness + r.led1_brightness) {
            turn_on(r.led2_wiring);
        }
    }
}

void turn_on( led_wiring led ) {
    // Update the pins' I/O state and value to include the requested LED.
    DDRB = DDRB | led.from.portB | led.to.portB;
    PORTB = PORTB | led.from.portB;
    DDRC = DDRC | led.from.portC | led.to.portC;
    PORTC = PORTC | led.from.portC;
    DDRD = DDRD | led.from.portD | led.to.portD;
    PORTD = PORTD | led.from.portD;
}

void turn_off() {
    // Turn off all the LEDs.
    DDRB = 0;
    PORTB = 0;
    DDRC = 0;
    PORTC = 0;
    DDRD = 0;
    PORTD = 0;
}

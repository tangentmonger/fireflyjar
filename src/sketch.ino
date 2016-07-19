#include <math.h>

#define BROWN_PIN 2
#define GREEN_PIN 3
#define YELLOW_PIN 4

#define BROWN pow(2, BROWN_PIN)
#define GREEN pow(2, GREEN_PIN)
#define YELLOW pow(2, YELLOW_PIN)

#define LED_COUNT 6
// PORTS:
//  B (digital pin 8 to 13)
//  C (analog input pins)
//  D (digital pins 0 to 7)


//PORTB = high/low state of digital pins 8-13, 1=high
//DDRB = Data Direction Register B. read/write state of digital pins 8-13, 1=write (output)
//PORTD = high/low state of digital pins 0-7, 1=high
//DDRD = Data Direction Register D. read/write state of digital pins 0-7, 1=write (output)

byte ledarray[LED_COUNT][2] = {
    {GREEN, YELLOW}, //lowest led
    {BROWN, YELLOW},
    {BROWN, GREEN},
    {YELLOW, GREEN},
    {YELLOW, BROWN},
    {GREEN, BROWN}
};


void turnOn( byte led ) {
    DDRD = ledarray[led][0] | ledarray[led][1];
    PORTD = ledarray[led][1];
}

void setup() {}

void loop() {
    for( byte led = 0; led < LED_COUNT; led++ ) {
        turnOn( led );
        delay( 100 );
    }
}

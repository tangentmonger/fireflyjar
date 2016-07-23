#include <math.h>

#define BLUE_PIN 2
#define GREEN_PIN 3
#define YELLOW_PIN 4
#define RED_PIN 5

#define BLUE pow(2, BLUE_PIN)
#define GREEN pow(2, GREEN_PIN)
#define YELLOW pow(2, YELLOW_PIN)
#define RED pow(2, RED_PIN)

#define LED_COUNT 12
// PORTS:
//  B (digital pin 8 to 13)
//  C (analog input pins)
//  D (digital pins 0 to 7)


//PORTB = high/low state of digital pins 8-13, 1=high
//DDRB = Data Direction Register B. read/write state of digital pins 8-13, 1=write (output)
//PORTD = high/low state of digital pins 0-7, 1=high
//DDRD = Data Direction Register D. read/write state of digital pins 0-7, 1=write (output)

byte ledarray[LED_COUNT][2] = {
    {RED, GREEN},
    {GREEN, RED},
    {GREEN, BLUE},
    {GREEN, YELLOW},
    {YELLOW, GREEN},
    {YELLOW, RED},
    {YELLOW, BLUE},
    {BLUE, YELLOW},
    {BLUE, GREEN},
    {BLUE, RED},
    {RED, BLUE},
    {RED, YELLOW},
};


void turnOn( byte led ) {
    DDRD = ledarray[led][0] | ledarray[led][1];
    PORTD = ledarray[led][0]; //colour on +ve led sides
}

void turnOff() {
    DDRD = 0;
    PORTD = 0;
}

void setup() {}

void loop() {
    for( byte led = LED_COUNT-1; led !=255; led-- ) { //8 bits unsigned, lol
        turnOn( led );
        delay( 30 );
    }

    turnOff();
    delay(2000);
}

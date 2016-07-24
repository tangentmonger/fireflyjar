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

const unsigned int PWM_LEVELS = 100; // also us in duty cycle

void turnOn( int led ) {
    DDRD = ledarray[led][0] | ledarray[led][1];
    PORTD = ledarray[led][0]; //colour on +ve led sides
}

void turnOff() {
    DDRD = 0;
    PORTD = 0;
}

void setup() {
    turnOff();
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

void show(int position, int brightness, long ms_duration) {
    //position is % of bug, with 0 at the bottom
    //brightness is % of max brightness

    //invert, so that 0 is the bottom
    position = 100-position;

    // adjust brightness
    brightness = sCurvedBrightness(brightness);

    // 1) which LEDs do we light?
    float virtual_led = float(LED_COUNT-1) * float(position) / 100.0;
    int lower_led = int(floor(virtual_led));
    int upper_led = int(ceil(virtual_led));

    // 2) in what proportions (as brightness percentage)?
    //assumtion: brightness is additive
    float proportion = virtual_led - float(lower_led);
    int lower_led_brightness = int(float(brightness) * (1-proportion));
    int upper_led_brightness = int(float(brightness) * proportion);

    // 3) ok, do that using PWM...
    long us_duration = ms_duration * 1000;
    long pulses = us_duration / PWM_LEVELS;
   
    for (long pulse=0; pulse<=pulses; pulse++){
        //do one pulse
        turnOn(lower_led);
        delayMicroseconds(lower_led_brightness+1); //can't delay by 0us :/
        turnOn(upper_led);
        delayMicroseconds(upper_led_brightness+1);
        turnOff();
        delayMicroseconds(100-lower_led_brightness-upper_led_brightness+1);
    }
}

void loop() {
    for(int position=0; position<50; position++)
    {
        show(position, position*2, 5);
    }
    for(int position=0; position<50; position++)
    {
        show(position+50, (50-position)*2, 5);
    }
    delay(1000);
}

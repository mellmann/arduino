#include <Adafruit_DotStar.h>

// Because conditional #includes don't work w/Arduino sketches...
//#include <SPI.h>     // COMMENT OUT THIS LINE FOR GEMMA OR TRINKET
#include <avr/power.h> // ENABLE THIS LINE FOR GEMMA OR TRINKET


#define NUMPIXELS 30 // Number of LEDs in strip

// Here's how to control the LEDs from any two pins:
#define DATAPIN    0
#define CLOCKPIN   1

Adafruit_DotStar strip = Adafruit_DotStar(
  NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_RGB);
// The last parameter is optional -- this is the color data order of the
// DotStar strip, which has changed over time in different production runs.
// Your code just uses R,G,B colors, the library then reassigns as needed.
// Default is DOTSTAR_BRG, so change this if you have an earlier strip.

// Hardware SPI is a little faster, but must be wired to specific pins
// (Arduino Uno = pin 11 for data, 13 for clock, other boards are different).
//Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DOTSTAR_BRG);


void setup() {

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
#endif

  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP

  //Serial.begin(9600);
}

// control parameter for the brightness
uint32_t max_brightness = 10;
uint32_t max_delay = 50;

// pulse shape for the spine animation
const int puls_length = 12;
uint32_t colorf[puls_length] = 
{
  1,
  60,
  128,
  255,
  200,
  160,
  128,
  80,
  60,
  40,
  10,
  1
};

// spine animation
int puls_position = -puls_length;


void setPixel(uint16_t n, uint8_t value) {
    strip.setPixelColor(n,0,value,0);
}

void setPixel(int first, int last, uint8_t value) {
  for(int i = max(first,0); i < min(last, NUMPIXELS); ++i) {
    setPixel(i, value);
  }
}

void spineAnimation() 
{
  if(++puls_position > NUMPIXELS) {
    setPixel(puls_position, 0); // off
    puls_position = -puls_length;
    delay(max(1,max_delay)*40+200);
  }
  
  for(int i = 0; i < puls_length; i++) {
    int x = puls_position + i + 1;
    if(x >= 0 && x < NUMPIXELS) {
      uint32_t c = map(colorf[12-i-1], 0, 255, 0, max_brightness);
      
      if(colorf[12-i-1] > 0) {
        c = max(1,c);
      }
      setPixel(x, c);
    }
  }
  
  setPixel(puls_position, 0); // off
  
  strip.show(); // Refresh strip

  float f = 1.0f - float(puls_position) / float(NUMPIXELS);
  int32_t d = int32_t(f*f*float(max_delay));
  delay(max(0,d));
}

void blinkAnimation() 
{
  
}


void loop() {

  // read the control values
  int val = analogRead(3); // #3?
  //Serial.print(val);
  //Serial.print(", ");
  max_brightness = map(val, 0, 1023, 0, 255);
  
  int val2 = analogRead(1); // #2
  //Serial.println(val2);
  max_delay = map(val2, 0, 1023, 0, 60);
  
  spineAnimation();
}



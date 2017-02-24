#include <Adafruit_DotStar.h>

// Because conditional #includes don't work w/Arduino sketches...
#include <SPI.h>     // COMMENT OUT THIS LINE FOR GEMMA OR TRINKET
//#include <avr/power.h> // ENABLE THIS LINE FOR GEMMA OR TRINKET


#define NUMPIXELS 72 // Number of LEDs in strip

// Here's how to control the LEDs from any two pins:
#define DATAPIN    A0
#define CLOCKPIN   A1

Adafruit_DotStar strip = Adafruit_DotStar(
  NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_RGB);
// The last parameter is optional -- this is the color data order of the
// DotStar strip, which has changed over time in different production runs.
// Your code just uses R,G,B colors, the library then reassigns as needed.
// Default is DOTSTAR_BRG, so change this if you have an earlier strip.

// Hardware SPI is a little faster, but must be wired to specific pins
// (Arduino Uno = pin 11 for data, 13 for clock, other boards are different).
//Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DOTSTAR_BRG);


// include some our tools
#include "MyTimer.h"


void setup() {

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
#endif

  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP

  //Serial.begin(9600);
}

// control parameter for the brightness
uint32_t max_brightness = 128;
uint32_t max_delay = 37;

// animation state
enum AnimationState 
{
  LOADING,
  SIGNAL_DONE_LOADING,
  RUN_SPINE
} animation_state;

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


// blink animation
const int blinkTimeOn = 300; // in ms
const int blinkTimeOff = 500; // in ms
const int blinkNumber = 4;
// 
int blinkCount = 0; // number of executed blinks
bool blinkState = false; // false ~ off, true ~on
int blinkBegin = 0;
int blinkEnd = NUMPIXELS;

MyTimer timer; // used by blink


// load animation
const int loadStateMin = 36; // start at 50%
int loadState = loadStateMin;
int loadStep = 3;
int maxLoadStep = 3;



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
  // turn on
  if(!blinkState && timer.getDuration() > blinkTimeOff) {
    blinkState = true;
    timer.reset();
    setPixel(blinkBegin, blinkEnd, 128);
    blinkCount++; // count the on states
  }
  
  // turn off
  if(blinkState && timer.getDuration() > blinkTimeOn) {
    blinkState = false;
    timer.reset();
    setPixel(blinkBegin, blinkEnd, 0);
    blinkCount++;
  }
}


void loadAnimation()
{
  // blink the next pixels to be loaded
  blinkBegin = loadState;
  blinkEnd   = loadState + loadStep;

  if(blinkCount < 2*blinkNumber+1) {
    blinkAnimation();
  } else {
    loadState = min(loadState + loadStep, NUMPIXELS);
    loadStep = (int)random(1, maxLoadStep + 1);
    // reset the blink state
    blinkState = false;
    blinkCount = 0;
    timer.reset();
  }
}

void loop() {

  // read the control values
  int val = analogRead(3); // #3?
  //Serial.print(val);
  //Serial.print(", ");
  //max_brightness = map(val, 0, 1023, 0, 255);
  
  int val2 = analogRead(1); // #2
  //Serial.println(val2);
  //max_delay = map(val2, 0, 1023, 0, 60);
  
  switch(animation_state)
  {
    case LOADING:
      loadAnimation();
      strip.show();
      if(loadState == NUMPIXELS) { // full
        animation_state = SIGNAL_DONE_LOADING;
      }
      break;
    case SIGNAL_DONE_LOADING: 
      blinkBegin = 0;
      blinkEnd   = NUMPIXELS;
      blinkAnimation();
      strip.show();
      if(blinkCount > 2*blinkNumber+1) {
        animation_state = RUN_SPINE;
        timer.reset();
      }
      break;
    case RUN_SPINE: 
      spineAnimation();
      if(timer.getDuration() > 15000 && puls_position == 0) {
        timer.reset();
        animation_state = LOADING;
        setPixel(0,NUMPIXELS,0);
        loadState = loadStateMin;
      }
      break;
    default: break; // should never happen
  }
}



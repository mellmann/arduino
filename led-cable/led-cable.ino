// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define NUMPIXELS 300 // Number of LEDs in strip

// Here's how to control the LEDs from any two pins:
#define DATAPIN    2

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, DATAPIN, NEO_GRB + NEO_KHZ800);


// Runs 10 LEDs at a time along strip, cycling through red, green and blue.
// This requires about 200 mA for all the 'on' pixels + 1 mA per 'off' pixel.

uint32_t max_brightness = 15;
uint32_t max_delay = 36;

uint32_t color = 128;
uint32_t fade  = 0;

uint32_t t = 0;

const int template_length = 12;
uint32_t colorTemplate[template_length] = 
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

const int factor = 4;
const int puls_length = template_length*factor;
uint32_t* colorf = new uint32_t[puls_length];

int puls_position = -puls_length;


void setup() {
  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP

  for(int i = 0; i < template_length; ++i) {
    for(int j = 0; j < factor; ++j) {
      colorf[i*factor + j] = colorTemplate[i];
    }
  }
}


void setB(uint16_t n, uint8_t value) {
    strip.setPixelColor(n,value,value,value);
}

uint8_t getB(uint16_t n) {
  return (uint8_t)(strip.getPixelColor(n));
}

void spineControl() 
{
  if(++puls_position > NUMPIXELS) {
    setB(puls_position, fade);
    puls_position = -puls_length;
    delay(max(1,max_delay)*40+200);
  }
  
  for(int i = 0; i < puls_length; i++) {
    int x = puls_position + i + 1;
    if(x >= 0 && x < NUMPIXELS) {
      uint32_t c = map(colorf[puls_length-i-1], 0, 255, 0, max_brightness);
      
      if(colorf[puls_length-i-1] > 0) {
        c = max(1,c);
      }
      setB(x, c);
    }
  }
  
  //strip.setPixelColor(head, color); // 'On' pixel at head
  setB(puls_position, fade);
  
  strip.show();                     // Refresh strip

  float f = 1.0f - float(puls_position) / float(NUMPIXELS);
  //int32_t d = int32_t(((exp(f)-1.0f)/1.71f)*float(max_delay));
  int32_t d = int32_t(f*f*float(max_delay));
  delay(max(0,d));
}


void loop() {
  
  int val = analogRead(3); // #3?
  //Serial.print(val);
  //Serial.print(", ");
  //max_brightness = map(val, 0, 1023, 0, 255);
  
  int val2 = analogRead(1); // #2
  //Serial.println(val2);
  //max_delay = map(val2, 0, 1023, 0, 60);
  
  spineControl();
}



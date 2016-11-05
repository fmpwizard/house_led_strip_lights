#include <Adafruit_NeoPixel.h>
#include "Arduino.h"

#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 11

//On off with ultrasonic distance sensor
//RGB LED on pin 9 to indicate if lights are on or off


//======================================

// Read pin from moteino
int inPin = 9;   // pushbutton connected to digital pin 7
//int val = 0;     // variable to store the read value //Diego
int val = 1;     // variable to store the read value
//======================================

boolean on = false; //Diego

//======================================

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(150, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code

  Serial.begin(9600);
  pinMode(inPin, INPUT);      // sets the digital pin 10 as input

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

//=============================

void lightsOff(){
  uint16_t x;
  for(x=0; x < strip.numPixels(); x++){
    strip.setPixelColor(x, strip.Color(0, 0, 0));
  }
  strip.show();
  on = false;
  delay(2000);
}

void checkPinIn(uint16_t round) {
  if (round % 10 == 0) {
    //uint8_t val = digitalRead(inPin); //Diego
    //if (val == 0 && on == true){
    if (false && on == true){
      lightsOff();
      return;
    }
  }
}

// Fill the dots one after the other with a color from corners to the middle
void colorWipe(uint32_t c, uint8_t wait) {
  if (val == 0) {
    on = false;
    return;
  }
  for(uint16_t s=0; s < (strip.numPixels()/2) + 1; s++) {
    strip.setPixelColor(s, c);
    strip.setPixelColor(strip.numPixels() - s, c);
    strip.show();
    delay(wait);
    checkPinIn(s);
  }
}

// Fill the dots one after the other with a color from middle out
void colorWipeInOut(uint32_t c, uint8_t wait) {
  if (val == 0) {
    on = false;
    return;
  }
  for(uint16_t s=strip.numPixels()/2; s >0 ; s--) {
    strip.setPixelColor(s, c);
    strip.setPixelColor(strip.numPixels() - s, c);
    strip.show();
    delay(wait);
    checkPinIn(s);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    checkPinIn(j);
    delay(wait);
  }
}



//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint16_t wait, uint8_t cycles) {
  for (int j=0; j<cycles; j++) {  //do N cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
      checkPinIn(j);
      delay(wait);
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}


//Theatre-style crawling lights with rainbow effect from corners to middle
void theaterChaseRainbowMiddle(uint8_t wait) {
  uint16_t i, j, q, x;
  for (j=0; j < 256; j=j+30) {     // cycle colors in the wheel, skipping every 30
    for (q=0; q < 3; q++) {
      for (i=0; i < strip.numPixels() /2; i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      for (x=strip.numPixels(); x > strip.numPixels() /2; x=x-3) {
        strip.setPixelColor(x-q, Wheel( (x-j) % 255));    //turn every third pixel on
      }
      strip.show();
      checkPinIn(j);
      delay(wait);
      for (i=0; i < strip.numPixels() / 2; i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
      for (i=strip.numPixels(); i > strip.numPixels() / 2; i=i-3) {
        strip.setPixelColor(i-q, 0);        //turn every third pixel off
      }
    }
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;
  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    checkPinIn(j);
    delay(wait);
  }
}
//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  uint16_t i, j, q;
  for (j=0; j < 256; j=j+20) {     // cycle all 256 colors in the wheel
    for (q=0; q < 3; q++) {
      for (i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();
      checkPinIn(j);
      delay(wait);
      for (i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

void splash(uint32_t c, uint8_t wait, uint8_t cnt) {
  for (uint8_t count; count < cnt; count ++) {
    for (uint16_t x = 0; x < strip.numPixels(); x ++) {
      strip.setPixelColor(x, c);
    }
    strip.show();
    checkPinIn(10);
    delay(wait);
    for (uint16_t x = 0; x < strip.numPixels(); x++) {
      strip.setPixelColor(x, 0);
    }
    strip.show();
    delay(wait);
  }
}

void loop() {

  //val = digitalRead(inPin); diego
  if (true){
  //if (val == 1){
    on = true;
    colorWipeInOut(strip.Color(255, 10, 100), 10);
    colorWipeInOut(strip.Color(100, 0, 255), 10);
    colorWipeInOut(strip.Color(0, 150, 255), 10);
    colorWipe(strip.Color(255, 10, 100), 10);
    colorWipe(strip.Color(100, 0, 255), 10);
    colorWipe(strip.Color(0, 150, 255), 10);
    rainbowCycle(1);
    theaterChase(strip.Color(255, 0, 0), 100, 20); // Red
    theaterChase(strip.Color(0, 0, 255), 100, 20); // Blue
    theaterChase(strip.Color(0, 255, 0), 100, 20); // Green
    theaterChaseRainbowMiddle(100);
    theaterChaseRainbow(100);
    splash(strip.Color(255, 255, 200), 100, 10);
    rainbow(1);

  }
  if (val == 0 && on == true) {
    lightsOff();
  }
}

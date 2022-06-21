/*
 * File: M5Stamp-3CU_LED.ino
 * 
 * Controlling Neopixel LED on M5Stamp-3CU
 * used Board: STAMP-C3
 * 
 * 2022-05-21/Claus Kühnel info@ckuehnel.ch
 */
#include <Adafruit_NeoPixel.h>

#define PIN       2 
#define NUMPIXELS 1 

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels

void setup() 
{
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
}

void loop() 
{
  pixels.clear();  pixels.show();
  delay(DELAYVAL);
  pixels.setPixelColor(0, pixels.Color(0, 150, 0)); pixels.show();   
  delay(DELAYVAL); 
  pixels.setPixelColor(0, pixels.Color(150, 0, 0)); pixels.show();   
  delay(DELAYVAL);
  pixels.setPixelColor(0, pixels.Color(0, 0, 150)); pixels.show();   
  delay(DELAYVAL);
}

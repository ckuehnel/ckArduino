/*
 * File: 01Space_ESP32-C3_SHTC3.ino
 *
 * Get values for temperatur & hunmidity from Sparfun SHTC3 breakout board
 * and measure battery voltage. Need voltage divider on GPI  
 * 
 */

#include <Arduino.h>
#include "esp_adc_cal.h"
#include <U8g2lib.h>
#include <Adafruit_NeoPixel.h>
#include <SparkFun_SHTC3.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
#define SDA_PIN 5
#define SCL_PIN 6
#define RGB_PIN 2

#define BAT_ADC 3   // Voltage divider for ADC Input to 01Spce ESP32-C3
#define DEBUG   1

float Voltage = 0.0;
uint32_t readADC_Cal(int ADC_Raw);
char buffer[10];

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel pixel = Adafruit_NeoPixel(1, RGB_PIN, NEO_GRB + NEO_KHZ800);

/*
  U8g2lib Example Overview:
    Frame Buffer Examples: clearBuffer/sendBuffer. Fast, but may not work with all Arduino boards because of RAM consumption
    Page Buffer Examples: firstPage/nextPage. Less RAM usage, should work with all Arduino boards.
    U8x8 Text Only Example: No RAM usage, direct communication with display controller. No graphics, 8x8 Text only.
*/

// Please UNCOMMENT one of the contructor lines below
// U8g2 Contructor List (Frame Buffer)
// The complete list is available here: https://github.com/olikraus/u8g2/wiki/u8g2setupcpp
// Please update the pin numbers according to your setup. Use U8X8_PIN_NONE if the reset pin is not connected

U8G2_SSD1306_72X40_ER_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);   // EastRising 0.42" OLED

SHTC3 mySHTC3;              // Declare an instance of the SHTC3 class

void setup(void) 
{
  if (DEBUG) Serial.begin(115200);
  delay(1000); // Wait for Serial Monitor
  if (DEBUG) Serial.println("Starting 01Space ESP32-C3...");
    
  Wire.begin(SDA_PIN, SCL_PIN);
  u8g2.begin();
  pixel.begin();
  pixel.setBrightness(50);
  pixel.show(); // Initialize all pixels to 'off'
  errorDecoder(mySHTC3.begin());                              // To start the sensor you must call "begin()", the default settings use Wire (default Arduino I2C port)
}

void loop(void) 
{
  Voltage = (readADC_Cal(analogRead(BAT_ADC))) * 2;
  if (DEBUG) Serial.printf("Vbat = %.0f mV\n", Voltage); // Print Voltage (in mV)
  SHTC3_Status_TypeDef result = mySHTC3.update();        // Call "update()" to command a measurement, wait for measurement to complete, and update the RH and T members of the object
  printInfo();
  pixel.setPixelColor(0, 127, 127, 127); // White
  pixel.show(); delay(20);
  pixel.setPixelColor(0, 0, 0, 0);      // Blank
  pixel.show();
  u8g2.clearBuffer();               // clear the internal memory
  u8g2.sendBuffer();                // transfer internal memory to the display
  u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
  delay(1000);
  float temp = mySHTC3.toDegC(); sprintf (buffer, "%.1f", temp); strcat(buffer, " °C");
  u8g2.drawStr(0,10,buffer); // write something to the internal memory
  u8g2.sendBuffer();                // transfer internal memory to the display
  delay(1000);
  float humi = mySHTC3.toPercent(); sprintf (buffer, "%.0f", humi); strcat(buffer, " %rH");
  u8g2.drawStr(0,25,buffer);    // write something to the internal memory
  u8g2.sendBuffer();                // transfer internal memory to the display
  delay(1000); sprintf (buffer, "%.0f", Voltage); strcat(buffer, " mV");
  u8g2.drawStr(0,40, buffer);    // write something to the internal memory
  u8g2.sendBuffer();                // transfer internal memory to the display
  delay(1000);
}

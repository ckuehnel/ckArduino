/*
 * File: ESP32-C3_Blink.ino
 * 
 * Turns an LED on for one second, then off for one second, repeatedly.
 * 
 * based on https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink
 */

#define LED_BUILTIN 3  // Green LED at GPIO3 on T-OI plus ESP32-C3

void setup() 
{
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() 
{
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}

/*
 * File: ESP32-C3_I2C_Test.ino
 * Test I2C bus on ESP32-C3
 * 
 */
#include <Wire.h>
#include <i2cdetect.h>

#define DEBUG 1
#define SDA 18  // do not use the Grove port
#define SCL 19

void setup() 
{
  Serial.begin(115200);
  delay(1000);              // wait for serial monitor
  if (DEBUG) Serial.println(F("Initializing..."));
  Serial.println("Test I2C Bus...");
  if (DEBUG) Serial.println(F("Running...")); //last line in setup()
}

void loop() 
{
  Wire.begin(SDA, SCL);
  Serial.println("\nScanning I2C Bus");
  i2cdetect(1,127);
  delay(2000);
}

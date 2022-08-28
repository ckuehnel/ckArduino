/*
 * File: FireBeetle ESP32-C3 Blink.ino
 * 
 */

const int led = 10; // Blue LED 
byte count = 0;

void setup() 
{
  Serial.begin(115200);
  delay(2000); // wait for serial monitor
  Serial.println("FireBeetle ESP32-C3 starts...");
  pinMode(led,OUTPUT);
}

void loop() 
{
  Serial.print("*");
  count++;
  if (count > 20)
  {
    Serial.println();
    count = 0;
  }

  digitalWrite(led,HIGH);
  delay(1000);
  digitalWrite(led,LOW);
  delay(1000);
}

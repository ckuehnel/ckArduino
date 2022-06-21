/*
 * File: M5Stamp-C3U_ADC.ino
 * 
 * Using ADC of M5Stamp-C3U
 * used Board: STAMP-C3U
 * 
 * 2022-05-27/Claus Kühnel info@ckuehnel.ch
 */
float conversionFactor = 3300./4096;

void setup() 
{
  Serial.begin(115200);
  delay(2000); // wait for serial monitor
  Serial.println("M5Stamp-C3U ADC");
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db); 
}

void loop() 
{
  // read the analog / millivolts value for GPIO2:
  int analogValue = analogRead(3); // GPIO3 used for ADC
  int analogVolts = (int) (analogValue * conversionFactor) + .5;
  float temp = temperatureRead();
  
  // print out the values you read:
  Serial.printf("ADC analog value = %d\n",analogValue);
  Serial.printf("ADC millivolts value = %d\n",analogVolts);
  Serial.printf("Core temperature = %3.1f °C\n\n", temp);
  
  delay(2000);  // delay in between reads for clear read from serial
}

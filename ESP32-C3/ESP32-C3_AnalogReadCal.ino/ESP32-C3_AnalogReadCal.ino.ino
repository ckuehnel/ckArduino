/*
 * File: ESP32-C3_AnalogReadCal.ino
 *  
 * Using ADC of T-OI plus ESP32-C3
 * 
 * 2022-05-27/Claus Kühnel info@ckuehnel.ch
 */
#include "Arduino.h"
#include "esp_adc_cal.h"

#define BAT_ADC    2

float Voltage = 0.0;
uint32_t readADC_Cal(int ADC_Raw);

void setup()
{
  Serial.begin(115200);
  delay(2000); // wait for serial monitor
  Serial.println("Measuring Vbat of T-OI plus ESP32-C3");

  Serial.println("Is ADC reference voltage have been burned to the eFuse?");
  if (ESP_OK == esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF)) 
    Serial.println("Calibration mode is supported in eFuse");
  else 
    Serial.println("Error, eFuse values are not burned");

  Serial.println("Are Two Point values have been burned to the eFuse?");
  if (ESP_OK == esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP)) 
    Serial.println("Calibration mode is supported in eFuse");
  else 
    Serial.println("Error, eFuse values are not burned");

  Serial.println("Which type of calibration value is used in characterization?");
  esp_adc_cal_characteristics_t adc_chars;
  esp_adc_cal_value_t calValue = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
  switch (calValue) 
  {
    case 0: Serial.println("Characterization based on reference voltage stored in eFuse");
            break;
    case 1: Serial.println("Characterization based on Two Point values stored in eFuse");
            break;
    case 2: Serial.println("Characterization based on default reference voltage");
            break;
    default: Serial.println("Type not specified");
  }
}

void loop()
{
  Voltage = (readADC_Cal(analogRead(BAT_ADC))) * 2;
  Serial.printf("Vbat = %.0f mV\n", Voltage); // Print Voltage (in mV)
  delay(1000);
}

uint32_t readADC_Cal(int ADC_Raw)
{
  esp_adc_cal_characteristics_t adc_chars;
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars); 
  return (esp_adc_cal_raw_to_voltage(ADC_Raw, &adc_chars));
}

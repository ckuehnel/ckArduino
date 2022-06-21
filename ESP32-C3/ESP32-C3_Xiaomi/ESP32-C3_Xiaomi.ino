/*    
 * File ESP32-C3_Xiaomi.ino    
 * 
 * Read temperature, humidity and battery level from Xiaomi MJ-TH-V1 BLE sensor w/ M5Stack Core2 via BLE   
 * based on http://educ8s.tv/esp32-xiaomi-hack  
 * 
 * adapted to ESP32-C3-DevKitM-1
 * 2021-04-30 Claus Kühnel info@ckuehnel.ch
 */
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_system.h"
#include <sstream>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#define SCAN_TIME  10 // seconds

boolean METRIC = true; //Set true for metric system; false for imperial

BLEScan *pBLEScan;

void IRAM_ATTR resetModule()
{
    ets_printf("reboot\n");
    esp_restart();
}

       
float current_humidity = -100;
float previous_humidity = -100;
float current_temperature = -100;
float previous_temperature = -100;
float previous_batlevel = -100;
float current_batlevel = -100;


class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks 
{
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {
    if (advertisedDevice.haveName() && advertisedDevice.haveServiceData() && !advertisedDevice.getName().compare("MJ_HT_V1")) 
    {
      int serviceDataCount = advertisedDevice.getServiceDataCount();
      std::string strServiceData = advertisedDevice.getServiceData(0);

      uint8_t cServiceData[100];
      char charServiceData[100];

      strServiceData.copy((char *)cServiceData, strServiceData.length(), 0);

      Serial.printf("\nAdvertised Device: %s\n", advertisedDevice.toString().c_str());

      for (int i=0;i<strServiceData.length();i++) sprintf(&charServiceData[i*2], "%02x", cServiceData[i]);

      std::stringstream ss;
      ss << "fe95" << charServiceData;
            
      Serial.print("Payload:");
      Serial.println(ss.str().c_str());

      char eventLog[256];
      unsigned long value, value2;
      char charValue[5] = {0,};
      switch (cServiceData[11]) 
      {
        case 0x04:
          sprintf(charValue, "%02X%02X", cServiceData[15], cServiceData[14]);
          value = strtol(charValue, 0, 16);
          if(METRIC) current_temperature = (float)value/10;
          else       current_temperature = CelciusToFahrenheit((float)value/10);
          displayTemperature();  
          break;
        case 0x06:
          sprintf(charValue, "%02X%02X", cServiceData[15], cServiceData[14]);
          value = strtol(charValue, 0, 16);  
          current_humidity = (float)value/10;
          displayHumidity();                      
          Serial.printf("HUMIDITY_EVENT: %s, %d\n", charValue, value);
          break;
        case 0x0A:
          sprintf(charValue, "%02X", cServiceData[14]);
          value = strtol(charValue, 0, 16);  
          current_batlevel = (float)value;
          displayBatLevel();                  
          Serial.printf("BATTERY_EVENT: %s, %d\n", charValue, value);
          break;
        case 0x0D:
          sprintf(charValue, "%02X%02X", cServiceData[15], cServiceData[14]);
          value = strtol(charValue, 0, 16);      
          if(METRIC) current_temperature = (float)value/10;
          else       current_temperature = CelciusToFahrenheit((float)value/10);
          displayTemperature();               
          Serial.printf("TEMPERATURE_EVENT: %s, %d\n", charValue, value);                    
          sprintf(charValue, "%02X%02X", cServiceData[17], cServiceData[16]);
          value2 = strtol(charValue, 0, 16);
          current_humidity = (float)value2/10;
          displayHumidity();                                        
          Serial.printf("HUMIDITY_EVENT: %s, %d\n", charValue, value2);
          break;
        }
     }
   }
};

void setup() 
{
  Serial.begin(115200);
  Serial.println("ESP32-C3-DevKitM-1 XIAOMI Display");
 
  initBluetooth();
}

void loop() 
{
  char printLog[256];
  Serial.printf("\nStart BLE scan for %d seconds...", SCAN_TIME);
  BLEScan* pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  BLEScanResults foundDevices = pBLEScan->start(SCAN_TIME);
  int count = foundDevices.getCount();
  printf("Found device count : %d\n", count);
  delay(100);
}

void initBluetooth()
{
    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan(); //create new scan
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
    pBLEScan->setInterval(0x50);
    pBLEScan->setWindow(0x30);
}

void displayTemperature()
{
   if(current_temperature != previous_temperature)
   {
      Serial.print("T: ");
      Serial.print(convertFloatToString(current_temperature));
      
      if(METRIC) Serial.print("C");
      else       Serial.print("F");
      
      previous_temperature = current_temperature;
   }
}

void displayHumidity()
{
   if(current_humidity != previous_humidity)
   {
      Serial.print("H: ");
      Serial.print(convertFloatToString(current_humidity));
      Serial.print(" %");
      
      previous_humidity = current_humidity;
   }
}

void displayBatLevel()
{
   if(current_batlevel != previous_batlevel)
   {
      Serial.print("BatLvl: ");
      Serial.print(convertFloatToString(current_batlevel));
      Serial.print(" %");
      
      previous_humidity = current_humidity;
   }
}

String convertFloatToString(float f)
{
  String s = String(f,1);
  return s;
}

float CelciusToFahrenheit(float Celsius)
{
 float Fahrenheit=0;
 Fahrenheit = Celsius * 9/5 + 32;
 return Fahrenheit;
}

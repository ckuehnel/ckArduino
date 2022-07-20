 /* File: ESP32-C3_Battery_Monitor_BLE.ino
 *  
 * based on https://github.com/Xinyuan-LilyGO/LilyGo-T-OI-PLUS</blob/main/example/deepsleep/deepsleep.ino
 * Author: Pranav Cherukupalli <cherukupallip@gmail.com>
 * 
 * based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
 * Ported to Arduino ESP32 by Evandro Copercini
 * 
 * Sends battery voltage via BLE to Central Device
 * 
 * 2022-07-06/Claus Kuehnel info@ckuehnel.ch
 */
#include "Arduino.h"
#include "esp_adc_cal.h"

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
uint8_t txValue = 0;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

#define DEVICE "LilyGo-T-OI-PLUS"

//Bluetooth connect/disconnect processing. Triggered automatically when a connect/disconnect event occurs
class MyServerCallbacks: public BLEServerCallbacks 
{
  void onConnect(BLEServer* pServer) 
  { //This function will be executed when Bluetooth is connected
    Serial.println("BLE connected");
    deviceConnected = true;
  };

  void onDisconnect(BLEServer* pServer) 
  {  //This function will be executed when Bluetooth is disconnected
      Serial.println("BLE disconnected");
      deviceConnected = false;
      delay(500); // give the bluetooth stack the chance to get things ready
      pServer->startAdvertising(); // restart advertising
  }
};

/****************Data receiving section*************/
//Bluetooth receive data processing. Triggered automatically when data is received
class MyCallbacks: public BLECharacteristicCallbacks 
{
  void onWrite(BLECharacteristic *pCharacteristic) 
  {
    std::string rxValue = pCharacteristic->getValue(); //Receive data and assign it to rxValue

    if (rxValue == "INFO") Serial.println("This is LilyGo-T-OI-PLUS");    //Determine whether the received character is "INFO"

    /*  if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");
        for (int i = 0; i < rxValue.length(); i++){
          Serial.print(rxValue[i]);
        }
        Serial.println();
        Serial.println("*********");
      } */
    }
};

#define BAT_ADC    2   // Voltage divider for ADC Input on T-OI plus ESP32-C3
#define LED_BUILTIN 3  // Green LED at GPIO3 on T-OI plus ESP32-C3

#define DEBUG 1  // 0 - disable serial communication

float Voltage = 0.0;
uint32_t readADC_Cal(int ADC_Raw);

#define uS_TO_S_FACTOR 1000000ULL   /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  30         /* Time ESP32 will go to sleep (in seconds) */

// Method to print the reason by which ESP32 has been awaken from sleep
void print_wakeup_reason()
{
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

void setup()
{
  if (DEBUG) Serial.begin(115200);
  delay(1000); // Wait for Serial Monitor
  if (DEBUG) Serial.println("Starting LilyGo-T-OI-PLUS...");

  BLEBegin();  //Initialize Bluetooth

  pinMode(LED_BUILTIN, OUTPUT);
    
  if (DEBUG) 
  {
    print_wakeup_reason(); //Print the wakeup reason for ESP32
    print_adc_characteristics();
  }

  Voltage = (readADC_Cal(analogRead(BAT_ADC))) * 2;
  if (DEBUG) Serial.printf("Vbat = %.0f mV\n", Voltage); // Print Voltage (in mV)

  /*
  Next we decide what all peripherals to shut down/keep on
  By default, ESP32 will automatically power down the peripherals
  not needed by the wakeup source, but if you want to be a poweruser
  this is for you. Read in detail at the API docs
  http://esp-idf.readthedocs.io/en/latest/api-reference/system/deep_sleep.html
  Left the line commented as an example of how to configure peripherals.
  The line below turns off all RTC peripherals in deep sleep.
  */
  //esp_deep_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  //Serial.println("Configured all RTC Peripherals to be powered down in sleep");

  /*
  Now that we have setup a wake cause and if needed setup the
  peripherals state in deep sleep, we can now start going to
  deep sleep.
  In the case that no wake up sources were provided but deep
  sleep was started, it will sleep forever unless hardware
  reset occurs.
  */
  /*
  if (DEBUG) Serial.println("Going to sleep...");
  if (DEBUG) Serial.flush();
  //delay(10000); 
  esp_deep_sleep_start();
  if (DEBUG) Serial.println("This will never be printed"); */
}

void loop()
{
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  
  Voltage = (readADC_Cal(analogRead(BAT_ADC))) * 2;
  if (DEBUG) Serial.printf("Vbat = %.0f mV\n", Voltage); // Print Voltage (in mV)
  
   /****************Data transmitting section*************/
  if (deviceConnected) 
  { //If there is a Bluetooth connection, send data
    pTxCharacteristic->setValue("Hello");  //Send string
    pTxCharacteristic->notify();
    delay(10); // bluetooth stack will go into congestion, if too many packets are sent

    pTxCharacteristic->setValue("DFRobot");  //Send string
    pTxCharacteristic->notify();
    delay(10); // bluetooth stack will go into congestion, if too many packets are sent
  }

  digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)
  delay(15000);
}

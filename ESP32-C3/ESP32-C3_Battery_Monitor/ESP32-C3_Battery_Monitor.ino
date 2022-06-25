 /* File: ESP32-C3_Battery_Monitor.ino
 *  
 * based on https://github.com/Xinyuan-LilyGO/LilyGo-T-OI-PLUS</blob/main/example/deepsleep/deepsleep.ino
 * Author: Pranav Cherukupalli <cherukupallip@gmail.com>
 * 
 * Visualization of battery voltage by InfluxDB
 * 
 * 2022-06-25/Claus Kuehnel info@ckuehnel.ch
 */
#include "Arduino.h"
#include "esp_adc_cal.h"

#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
#include "arduino_secrets.h"

// InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

// Data point
Point sensor("T-OI-Plus Battery_Voltage");

#define BAT_ADC    2   // Voltage divider for ADC Input on T-OI plus ESP32-C3
#define LED_BUILTIN 3  // Green LED at GPIO3 on T-OI plus ESP32-C3

#define DEBUG 1  // 0 - disable serial communication

float Voltage = 0.0;
uint32_t readADC_Cal(int ADC_Raw);

#define uS_TO_S_FACTOR 1000000ULL   /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  1800         /* Time ESP32 will go to sleep (in seconds) */

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

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  
  if (DEBUG) 
  {
    print_wakeup_reason(); //Print the wakeup reason for ESP32
    print_adc_characteristics();
  }

  Voltage = (readADC_Cal(analogRead(BAT_ADC))) * 2;
  if (DEBUG) Serial.printf("Vbat = %.0f mV\n", Voltage); // Print Voltage (in mV)

  connect_InfluxDB();

  // Clear fields for reusing the point. Tags will remain untouched
  sensor.clearFields();

  // Store measured value into point
  // Report RSSI of currently connected network
  sensor.addField("VBAT", Voltage);

  // Print what are we exactly writing
  if (DEBUG) 
  {
    Serial.print("Writing: ");
    Serial.println(sensor.toLineProtocol());
  }

  // Check WiFi connection and reconnect if needed
  if (wifiMulti.run() != WL_CONNECTED) 
  {
    if (DEBUG) Serial.println("Wifi connection lost");
  }

  // Write point
  if (!client.writePoint(sensor)) 
  {
    if (DEBUG) 
    {
      Serial.print("InfluxDB write failed: ");
      Serial.println(client.getLastErrorMessage());
    }
  }
  
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  if (DEBUG) Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");

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
  if (DEBUG) Serial.println("Going to sleep...");
  if (DEBUG) Serial.flush();
  //delay(10000); 
  esp_deep_sleep_start();
  if (DEBUG) Serial.println("This will never be printed");
}

void loop()
{
  //This is not going to be called
}

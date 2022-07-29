/*
 * File: TTGO_GPS_LTE_DeepSleep.ino
 * 
 * 
 * based on TimerWakeUp by Pranav Cherukupalli <cherukupallip@gmail.com>
 */
#include <ArduinoJson.h>

#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  30          /* Sleep time (in seconds) */

RTC_DATA_ATTR int bootCount = 0;

// choose your modem
#define TINY_GSM_MODEM_SIM7000
#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb

// set GSM PIN, if any
#define GSM_PIN ""

#include <TinyGsmClient.h>
#include <SPI.h>

#define SerialAT     Serial1
#define UART_BAUD    9600
#define PIN_DTR      25
#define PIN_TX       27
#define PIN_RX       26
#define PWR_PIN      4

#define SD_MISO      2
#define SD_MOSI      15
#define SD_SCLK      14
#define SD_CS        13
#define LED_PIN      12
#define BAT_ADC      35

#define DEBUG        1

TinyGsm modem(SerialAT);

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

// GPS variables
float lat,  lon, speed, alt;
int usat;

// ENV.II variables
float sht31Temperature, sht31Humidity;
float bmp280Temperature, bmp280Pressure;

// MQTT variables
char msg[256];
size_t len = 0; 

void setup()
{
  Serial.begin(115200);
  delay(1000); // wait for serial monitor

  //Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("\nBoot number: " + String(bootCount));

  //Print the wakeup reason for ESP32
  print_wakeup_reason();

  // Configute wake up source and sleep time
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
  " Seconds");
  /******* Begin of application program ***************************************/
  // Set blue LED OFF
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  Serial.println("\nInitialsize SIM7000...");
  modemPowerOn();
  modem.restart();

  SerialAT.begin(UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);

  Serial.println("Initialization of ENV.II Unit...");
  initSensor();   // Initialization of M5Stack ENV.II unit

  delay(1000);
/*
  if (!modem.testAT()) 
  {
    Serial.println("Failed to restart modem, attempting to continue without restarting");
    modemRestart();
    return;
  }
*/
  float Vbat = readBattery(BAT_ADC);
  bool isUSBpowered = false;
  
  Serial.printf("Vbat  = %5.0f mV\n", Vbat);
  
  getValues();  // Read measuring values from M5Stck ENV.II unit
  
  if (! Vbat < 0.1) 
  {
    isUSBpowered = true;   // USB powered device
    Serial.printf("USB powered device\n");
    readGPS();
  }
  
  StaticJsonDocument<256> doc;
  doc["node"] = "TTGO-LTE";

  if (isUSBpowered)
  {
    String sLat = String(lat,2); doc["lat"] = sLat;   // conversion float to string
    String sLon = String(lon,2); doc["lon"] = sLon;
    String sAlt = String(alt,1); doc["alt"] = sAlt;
    String sSpd = String(speed,1); doc["spd"] = sSpd;
    String sUsat = String(usat); doc["usat"] = sUsat;
  }
  String sTemp = String(sht31Temperature,1); doc["temp"] = sTemp;
  String sHumi = String(sht31Humidity,0); doc["humi"] = sHumi;
  String sVbat = String(Vbat,0); doc["vbat"] = sVbat;
  String sRbt  = String(bootCount); doc["reboot"] = sRbt;

  // Generate the minified JSON and send it to the Serial port.
  serializeJsonPretty(doc, Serial);
  Serial.println();

  // Prepare the message to send by MQTT
  serializeJsonPretty(doc, msg);
  len = strlen(msg);
  Serial.printf("Message length = %d\n", len);

  connectLTE();
  blink(5);
  /******* End of application program ****************************************/

  /*
  Now that we have setup a wake cause and if needed setup the
  peripherals state in deep sleep, we can now start going to
  deep sleep.
  In the case that no wake up sources were provided but deep
  sleep was started, it will sleep forever unless hardware
  reset occurs.
  */
  Serial.println("\nGoing to sleep now");
  Serial.flush(); 
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}

void loop(){}

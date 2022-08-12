/*
 * File: TTGO_GPS_MQTT_Client.ino
 * 
 *
 */
#include <ArduinoJson.h>

#define TINY_GSM_MODEM_SIM7000

// Set serial for AT commands (to the module)
#define SerialAT Serial1

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG Serial

// Define how you're planning to connect to the internet
// These defines are only for this example; they are not needed in other code.
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false

// set GSM PIN, if any
#define GSM_PIN ""

// Your GPRS credentials, if any
const char apn[]      = "YourAPN";
const char gprsUser[] = "";
const char gprsPass[] = "";

// MQTT details
const char *broker = "broker.hivemq.com";

const char *topicUpdate      = "TTGO/update";
const char *topicSensor      = "TTGO/sensor";
const char *topicGPS         = "TTGO/gps";

#include <TinyGsmClient.h>
#include <PubSubClient.h>
#include <Ticker.h>
#include <SPI.h>
#include <SD.h>

// Just in case someone defined the wrong thing..
#if TINY_GSM_USE_GPRS && not defined TINY_GSM_MODEM_HAS_GPRS
#undef TINY_GSM_USE_GPRS
#undef TINY_GSM_USE_WIFI
#define TINY_GSM_USE_GPRS false
#define TINY_GSM_USE_WIFI true
#endif
#if TINY_GSM_USE_WIFI && not defined TINY_GSM_MODEM_HAS_WIFI
#undef TINY_GSM_USE_GPRS
#undef TINY_GSM_USE_WIFI
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false
#endif

TinyGsm       modem(SerialAT);
TinyGsmClient client(modem);
PubSubClient  mqtt(client);

Ticker tick;

#define uS_TO_S_FACTOR 1000000ULL  // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  60          // Time ESP32 will go to sleep (in seconds)

#define UART_BAUD   115200
#define PIN_DTR     25
#define PIN_TX      27
#define PIN_RX      26
#define PWR_PIN     4

#define SD_MISO     2
#define SD_MOSI     15
#define SD_SCLK     14
#define SD_CS       13
#define LED_PIN     12
#define BAT_ADC     35

#define DEBUG       1



// GPS variables
float lat,  lon, speed, alt;
int usat;

// ENV.II variables
float sht31Temperature, sht31Humidity;
float bmp280Temperature, bmp280Pressure;
bool sensorActive = true;
bool GPSActive = true;

// MQTT variables
char msg[256];
size_t len = 0; 

uint32_t lastReconnectAttempt = 0;
uint32_t lastMeasure = 0;

void mqttCallback(char *topic, byte *payload, unsigned int len)
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("]: ");
    Serial.write(payload, len);
    Serial.println();

    // Only proceed if incoming message's topic matches
    if (String(topic) == topicSensor) 
    {
      Serial.print("sensor: ");
      sensorActive = payload[0] - 0x30;  // payload 0 or 1
      Serial.println(sensorActive);
    }
    if (String(topic) == topicGPS) 
    {
      Serial.print("GPS: ");
      GPSActive = payload[0] - 0x30;  // payload 0 or 1
      Serial.println(GPSActive);
    }
}

boolean mqttConnect()
{
    Serial.print("Connecting to ");
    Serial.print(broker);

    // Connect to MQTT Broker
    boolean status = mqtt.connect("TTGO");

    if (status == false) 
    {
        Serial.println(" fail");
        return false;
    }
    Serial.println(" success");
    mqtt.publish(topicUpdate, msg);
    mqtt.subscribe(topicSensor);
    mqtt.subscribe(topicGPS);
    blink(5);
    return mqtt.connected();
}


void setup()
{
    // Set console baud rate
    Serial.begin(115200);
    delay(1000);  // wait for serial monitor
    Serial.println("\nStarting TTGO-GPS-LTE-MQTT Client...");

    // Set LED OFF
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);

    pinMode(PWR_PIN, OUTPUT);
    digitalWrite(PWR_PIN, HIGH);
    delay(300);
    digitalWrite(PWR_PIN, LOW);

    SPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
    if (!SD.begin(SD_CS)) {
        Serial.println("SDCard MOUNT FAIL");
    } else {
        uint32_t cardSize = SD.cardSize() / (1024 * 1024);
        String str = "SDCard Size: " + String(cardSize) + "MB";
        Serial.println(str);
    }

    Serial.println("Initializing...");

    delay(1000);

    SerialAT.begin(UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);

    // Restart takes quite some time
    // To skip it, call init() instead of restart()
    Serial.println("Initializing modem...");
    if (!modem.restart()) Serial.println("Failed to restart modem, attempting to continue without restarting");
/*
    String name = modem.getModemName();
    DBG("Modem Name:", name);

    String modemInfo = modem.getModemInfo();
    DBG("Modem Info:", modemInfo);
*/
#if TINY_GSM_USE_GPRS
    // Unlock your SIM card with a PIN if needed
    if (GSM_PIN && modem.getSimStatus() != 3) {
        modem.simUnlock(GSM_PIN);
    }
#endif

    Serial.print("Waiting for network...");
    if (!modem.waitForNetwork()) 
    {
        Serial.println(" fail");
        delay(10000);
        return;
    }
    Serial.println(" success");

    if (modem.isNetworkConnected()) Serial.println("Network connected");

#if TINY_GSM_USE_GPRS
    // GPRS connection parameters are usually set after network registration
    Serial.print(F("Connecting to "));
    Serial.print(apn);
    if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
        Serial.println(" fail");
        delay(10000);
        return;
    }
    Serial.println(" success");

    if (modem.isGprsConnected()) {
        Serial.println("GPRS connected");
    }
#endif

    // MQTT Broker setup
    mqtt.setServer(broker, 1883);
    mqtt.setCallback(mqttCallback);

    initSensor();   // Initialization of M5Stack ENV.II unit
    
    Serial.println("Initialization finished.");
}

void loop()
{
  uint32_t measure = millis();
  if (measure - lastMeasure > 60000L) // measuring cycle is 60 sec
  {
    lastMeasure = measure;

    blink(2);

    float Vbat = readBattery(BAT_ADC);
    bool isUSBpowered = false;
  
    Serial.printf("Vbat  = %5.0f mV\n", Vbat);

    if (sensorActive)
    {
      getValues();  // Read measuring values from M5Stck ENV.II unit
    }
    
    if (Vbat < 0.1) 
    {
      isUSBpowered = true;   // USB powered device
      Serial.printf("USB powered device\n");
    }

    if (GPSActive) readGPS();
 
    StaticJsonDocument<256> doc;
    doc["node"] = "TTGO-LTE";

    if (GPSActive)
    {
      String sLat = String(lat,2); doc["lat"] = sLat;   // conversion float to string
      String sLon = String(lon,2); doc["lon"] = sLon;
      String sAlt = String(alt,1); doc["alt"] = sAlt;
      String sSpd = String(speed,1); doc["spd"] = sSpd;
      String sUsat = String(usat); doc["usat"] = sUsat;
    }
    if (sensorActive)
    {
      String sTemp = String(sht31Temperature,1); doc["temp"] = sTemp;
      String sHumi = String(sht31Humidity,0); doc["humi"] = sHumi;
    }
    if (isUSBpowered) doc["vbat"] = "USB powered";
    else 
    {
      String sVbat = String(Vbat,0); doc["vbat"] = sVbat;
    }

    // Generate the minified JSON and send it to the Serial port.
    serializeJsonPretty(doc, Serial);
    Serial.println();

    // Prepare the message to send by MQTT
    serializeJsonPretty(doc, msg);
    len = strlen(msg);
    Serial.printf("Message length = %d\n", len);


    // Make sure we're still registered on the network
    if (!modem.isNetworkConnected()) {
        Serial.println("Network disconnected");
        if (!modem.waitForNetwork(180000L, true)) {
            Serial.println(" fail");
            delay(10000);
            return;
        }
        if (modem.isNetworkConnected()) {
            Serial.println("Network re-connected");
        }

#if TINY_GSM_USE_GPRS
        // and make sure GPRS/EPS is still connected
        if (!modem.isGprsConnected()) {
            Serial.println("GPRS disconnected!");
            Serial.print(F("Connecting to "));
            Serial.print(apn);
            if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
                Serial.println(" fail");
                delay(10000);
                return;
            }
            if (modem.isGprsConnected()) {
                Serial.println("GPRS reconnected");
            }
        }
#endif
    }
    //mqtt.disconnect();
    if (!mqtt.connected()) 
    {
        Serial.println("=== MQTT NOT CONNECTED ===");
        // Reconnect every 10 seconds
        uint32_t t = millis();
        if (t - lastReconnectAttempt > 10000L) 
        {
            lastReconnectAttempt = t;
            if (mqttConnect()) lastReconnectAttempt = 0;
        }
        delay(100);
        return;
    }
    else mqttConnect();
  }
  mqtt.loop();
}

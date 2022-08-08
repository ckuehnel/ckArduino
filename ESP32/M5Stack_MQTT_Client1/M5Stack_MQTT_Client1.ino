/*
 * File: M5Stack_MQTT_Client1.ino
 * 
 * based on https://github.com/survivingwithandroid/ESP32-MQTT
 *  & https://esp32-server.de/ntp/
 * 
 */
#include <M5Stack.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "arduino_secrets.h"

#define NTP_SERVER "de.pool.ntp.org"
#define TZ_INFO "WEST-1DWEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00" // Western European Time

// MQTT client
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient); 

void connectToWiFi() 
{
  Serial.print("Connecting to ");
 
  WiFi.begin(ssid, password);
  Serial.println(ssid);

  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print(".");
    delay(500);
  }

  Serial.println("\nConnected."); 
} 

void callback(char* topic, byte* payload, unsigned int length) 
{
  tm local;
  getLocalTime(&local);
  Serial.println(&local, "Date: %d.%m.%y  Time: %H:%M:%S"); // formatted output
  
  M5.Speaker.tone(2160); delay(50), M5.Speaker.end();
  Serial.println("Message received:");
  for (int i = 0; i < length; i++) 
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  
  M5.Lcd.fillScreen(BLACK);
  displayHeader();
  M5.Lcd.setCursor(10, 25);
  M5.Lcd.println("Message received @ ");
  M5.Lcd.println(&local, "        %d.%m.%y %H:%M:%S");
  for (int i = 0; i < length; i++) 
  {
    M5.Lcd.print((char)payload[i]);
  }
  M5.Lcd.println();
}

void setupMQTT() 
{
  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(callback);
}


void setup() 
{
  M5.begin();
  M5.Power.begin();
  M5.Speaker.begin(); //Initialize the speaker
  M5.Speaker.tone(1728); delay(50), M5.Speaker.end();
  delay(1000); // wait for serial monitor
  Serial.println("M5Stack MQTT Client");
  
  displayHeader();
  
  connectToWiFi();
  setupMQTT();  

  setenv("TZ", TZ_INFO, 1);          // set time zone
  tzset();
  Serial.println("Get NTP Time...");
  struct tm local;
  configTzTime(TZ_INFO, NTP_SERVER); // NTP Synchronization
  getLocalTime(&local, 10000);       // for 10 s
}

void reconnect() 
{
  Serial.println("Connecting to MQTT Broker...");
  while (!mqttClient.connected()) 
  {
    Serial.println("Reconnecting to MQTT Broker..");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
      
    if (mqttClient.connect(clientId.c_str())) 
    {
      Serial.println("Connected.");
      // subscribe to topic
      mqttClient.subscribe("TTGO/update");
    }   
  }
}


void loop() 
{
  
  if (!mqttClient.connected()) reconnect();
  mqttClient.loop();
}

/*
 * File: M5Stack_MQTT_Client1.ino
 * 
 * based on https://github.com/survivingwithandroid/ESP32-MQTT
 * 
 */
#include <M5Stack.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "arduino_secrets.h"

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
  M5.Speaker.tone(2160); delay(50), M5.Speaker.end();
  Serial.println("Message received:");
  for (int i = 0; i < length; i++) 
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  
  M5.Lcd.fillScreen(BLACK);
  displayHeader();
  M5.Lcd.setCursor(10, 40);
  M5.Lcd.println("Message received:");
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

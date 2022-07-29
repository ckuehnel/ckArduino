// Read GPS data

void readGPS()
{
  Serial.println("Search for GNSS sattelites...");

  enableGPS();

  uint8_t count = 0;
  while (1) 
  {
    if (modem.getGPS(&lat, &lon, &speed, &alt, &usat)) 
    {
      Serial.println("The location has been locked, the latitude and longitude are:");
      Serial.print("latitude:"); Serial.println(lat);
      Serial.print("longitude:"); Serial.println(lon);
      Serial.print("speed:"); Serial.println(speed);
      Serial.print("altitude:"); Serial.println(alt);
      Serial.print("GNSS satellites used:"); Serial.println(usat);
      break;
    }
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    count++;
    Serial.println(count);
    delay(2000);
  }
  disableGPS();
  modemPowerOff();
}

// Reading GPS data

void readGPS()
{
  enableGPS();

  int count = 0;
   
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
    count++;
    if (count > 50) 
    {
      Serial.printf("No GPS fix possible.\n");
      lat = lon = speed = alt = 0.;
      usat = 0;
      break;
    }
    Serial.printf("Count = %d\n", count);
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    delay(2000);
  }
  disableGPS();
}  

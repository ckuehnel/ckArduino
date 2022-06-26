///////////////////////
// Utility Functions //
///////////////////////
void printInfo()
{
  if(mySHTC3.lastStatus == SHTC3_Status_Nominal)              // You can also assess the status of the last command by checking the ".lastStatus" member of the object
  {
    if (DEBUG)
    {
      Serial.print("RH = "); 
      Serial.print(mySHTC3.toPercent(),0);                        // "toPercent" returns the percent humidity as a floating point number
      Serial.print(" %, T = "); 
      Serial.print(mySHTC3.toDegC(),1);                           // "toDegF" and "toDegC" return the temperature as a flaoting point number in deg F and deg C respectively 
      Serial.println(" °C"); 
    }
  }
  else
  {
    if (DEBUG)
    {
      Serial.print("Update failed, error: "); 
      errorDecoder(mySHTC3.lastStatus);
      Serial.println();
    }
  }
}

void errorDecoder(SHTC3_Status_TypeDef message)                             // The errorDecoder function prints "SHTC3_Status_TypeDef" resultsin a human-friendly way
{
  if (DEBUG)
  {
    switch(message)
    {
      case SHTC3_Status_Nominal : Serial.print("Nominal"); break;
      case SHTC3_Status_Error : Serial.print("Error"); break;
      case SHTC3_Status_CRC_Fail : Serial.print("CRC Fail"); break;
      default : Serial.print("Unknown return code"); break;
    }
  }
}

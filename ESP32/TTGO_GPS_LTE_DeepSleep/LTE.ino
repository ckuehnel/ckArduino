//

void connectLTE()
{
  String res;

  Serial.println("Modem Initialization...");
  modemPowerOn();
  modem.init();
/*  if (!modem.init()) 
  {
    modemRestart();
    delay(2000);
    Serial.println("Failed to restart modem, attempting to continue without restarting");
    return;
  }
*/
  modem.sendAT("+SIMCOMATI");
  modem.waitResponse(1000L, res);
  res.replace(GSM_NL "OK" GSM_NL, "");
  Serial.println(res);
 
  res = "";
  Serial.println();
  Serial.print("Preferred mode selection...");
  modem.sendAT("+CNMP?");
  if (modem.waitResponse(1000L, res) == 1) 
  {
    res.replace(GSM_NL "OK" GSM_NL, "");
    Serial.println(res);
  }
  
  res = "";
  Serial.print("Preferred selection between CAT-M and NB-IoT...");
  modem.sendAT("+CMNB?");
  if (modem.waitResponse(1000L, res) == 1) 
  {
    res.replace(GSM_NL "OK" GSM_NL, "");
    Serial.println(res);
  }

  String name = modem.getModemName();
  Serial.println("Modem Name: " + name);

  String modemInfo = modem.getModemInfo();
  Serial.println("Modem Info: " + modemInfo);

  // Unlock your SIM card with a PIN if needed
  if ( GSM_PIN && modem.getSimStatus() != 3 ) modem.simUnlock(GSM_PIN);

  /*
    2 Automatic
    13 GSM only
    38 LTE only
    51 GSM and LTE only
  */
  Serial.println("Set Network Mode to LTE-M");
  res = modem.setNetworkMode(38);

  /*
    1 CAT-M
    2 NB-Iot
    3 CAT-M and NB-IoT
  */
  Serial.println("Set Preffered Mode to Cat-M1");
  res = modem.setPreferredMode(1);

  digitalWrite(LED_PIN, LOW);
  bool isConnected = false;
  int tryCount = 60;
  while (tryCount--) 
  {
    int16_t signal =  modem.getSignalQuality();
    Serial.print("Signal: ");
    Serial.println(signal);
    Serial.print("Network connected? ");
    isConnected = modem.isNetworkConnected();
    Serial.println( isConnected ? "Connected" : "Not Connected");
    if (isConnected) break;
  }

  Serial.println();
  Serial.println("Device is connected.");
  Serial.println();

  res = "";
  Serial.println("Inquiring UE system information...");
  modem.sendAT("+CPSI?");
  if (modem.waitResponse(1000L, res) == 1) 
  {
    res.replace(GSM_NL "OK" GSM_NL, "");
    Serial.println(res);
  }

  res = "";
  Serial.println("Open wireless connection...");
  modem.sendAT("+CNACT=1,\"iot.1nce.net\"");
  if (modem.waitResponse(5000L, res) == 1) 
  {
    res.replace(GSM_NL "OK" GSM_NL, "");
    Serial.println(res);
  }

  res = "";
  Serial.println("Get local IP...");
  modem.sendAT("+CNACT?");
  if (modem.waitResponse(1000L, res) == 1) 
  {
    res.replace(GSM_NL "OK" GSM_NL, "");
    Serial.println(res);
  }

  res = "";
  Serial.println("Set up server URL...");
  modem.sendAT("+SMCONF=\"URL\",\"broker.hivemq.com\"");
  if (modem.waitResponse(1000L, res) == 1) 
  {
    res.replace(GSM_NL "OK" GSM_NL, "");
    Serial.println(res);
  }

  res = "";
  Serial.println("Set up ClientID...");
  modem.sendAT("+SMCONF=\"CLIENTID\",\"LTE\"");
  if (modem.waitResponse(1000L, res) == 1) 
  {
    res.replace(GSM_NL "OK" GSM_NL, "");
    Serial.println(res);
  }

  res = "";
  Serial.println("Set up Keep Time...");
  modem.sendAT("+SMCONF=\"KEEPTIME\",\"90\"");
  if (modem.waitResponse(1000L, res) == 1) 
  {
    res.replace(GSM_NL "OK" GSM_NL, "");
    Serial.println(res);
  }
  
  res = "";
  Serial.println("Set up QoS...");
  modem.sendAT("+SMCONF=\"QOS\",0");
  if (modem.waitResponse(1000L, res) == 1) 
  {
    res.replace(GSM_NL "OK" GSM_NL, "");
    Serial.println(res);
  }

  res = "";
  Serial.println("Read MQTT Parameter...");
  modem.sendAT("+SMCONF?");
  if (modem.waitResponse(1000L, res) == 1) 
  {
    res.replace(GSM_NL "OK" GSM_NL, "");
    Serial.println(res);
  }

  res = "";
  Serial.println("Connecting MQTT...");
  modem.sendAT("+SMCONN");
  if (modem.waitResponse(5000L, res) == 1) 
  {
    res.replace(GSM_NL "OK" GSM_NL, "");
    Serial.println(res);
  }
  
  res = "";
  Serial.println("Send packet...");
  modem.sendAT("+SMPUB=\"update\",\"" + String(len) + "\",1,1");
  delay(100);
  SerialAT.println(msg); 
  if (modem.waitResponse(2000L, res) == 1) 
  {
    res.replace(GSM_NL "OK" GSM_NL, "");
    Serial.println(res);
  }
   
  res = "";
  Serial.println("Disconnect MQTT...");
  modem.sendAT("+SMDISC");
  if (modem.waitResponse(1000L, res) == 1) 
  {
    res.replace(GSM_NL "OK" GSM_NL, "");
    Serial.println(res);
  }

  res = "";
  Serial.println("Disconnect wireless...");
  modem.sendAT("+CNACT=0");
  if (modem.waitResponse(1000L, res) == 1) 
  {
    res.replace(GSM_NL "OK" GSM_NL, "");
    Serial.println(res);
  }
  modemPowerOff();
}

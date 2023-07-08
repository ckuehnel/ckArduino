int led = 10;

void setup() 
{
  Serial.begin(115200);
  delay(1000);
  pinMode(led,OUTPUT);
}

void loop() 
{
  digitalWrite(led,HIGH);
  delay(1000);
  digitalWrite(led,LOW);
  delay(1000);
  Serial.print("*");
}

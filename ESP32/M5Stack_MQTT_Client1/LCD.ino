//  LCD routines

void displayHeader(void)
{
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setBrightness(50);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.printf("M5Stack MQTT Client");
}

//  LCD routines

void displayHeader(void)
{
  M5.Lcd.setTextColor(GREEN, BLACK);
  M5.Lcd.setBrightness(50);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(10, 5);
  M5.Lcd.printf("M5Stack MQTT Client");
  M5.Lcd.setTextColor(WHITE, BLACK);
}

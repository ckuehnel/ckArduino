/*
 * File: FireBeetle ESP32-C3 Internals.ino
 * 
 * Please select ESP32-C3 Dev Module via Tools > Board:
 * and select the serial port. 
 * USB CDC on Boot must be enabled for M5Stamp-3CU.
 * 
 * 2022-08-21 Claus Kühnel info@ckuehnel.ch
 */

void setup() 
{
  Serial.begin(115200);
  delay(2000); // wait for serial monitor
  Serial.println("FireBeetle ESP32-C3 - RISC-V microcontroller");
  Serial.printf("ESP32 Internals:\n");
  Serial.printf("ESP Arduino version %d.%d.%d\n", ESP_ARDUINO_VERSION_MAJOR, ESP_ARDUINO_VERSION_MINOR, ESP_ARDUINO_VERSION_PATCH);
  Serial.printf("Chip Revision %d\n", ESP.getChipRevision());
  Serial.printf("Chip model %s\n", ESP.getChipModel());
  Serial.printf("Number of cores %d\n", ESP.getChipCores());
  Serial.printf("CPU frequency %d MHz\n", ESP.getCpuFreqMHz());
  Serial.printf("Total heap %d KB\n", ESP.getHeapSize()/1024); 
  Serial.printf("Free  heap %d KB\n", ESP.getFreeHeap()/1024); 
  Serial.printf("PSRAM %d MB\n", ESP.getPsramSize()/1048576);
  Serial.printf("Free PSRAM %d MB\n", ESP.getFreePsram()/1048576);
  Serial.printf("Flash memory %d MByte\n", ESP.getFlashChipSize()/1048576);
}

void loop() {}
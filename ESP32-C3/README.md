# LilyGo T-OI plus RISC-V ESP32-C3

Program examples for ESP32-C3-DevModule

Before using the Grove connector look to \Images\T-OI plus Grove Connector.png. There is a misalignment on the board.

Further info @ http://www.lilygo.cn/prod_view.aspx?TypeId=50063&Id=1361&FId=t3:50063:3


# M5Stamp-C3U RISC-V ESP32-C3

Program examples for Espressif's M5Stamp-C3 modules

Pay attention to the follwing hints:

Enters program download mode operation for M5Stamp-C3U: 
1. Long press the center button (G9) of STAMP C3U when the power is off. 
2. Connect to the computer; after the port is successfully identified, program burning.

By default, USB CDC is not enabled, the serial port output started by C3U will be output through the default pins of UART0. If you want to output through USB, please use the IDE to output Its USB CDC option is enabled. Enable it through Tools->USB CDC on Boot-Enabled. 

Further info @ https://docs.m5stack.com/en/core/stamp_c3u


# 01Space-ESP32-C3 RISC-V 

Further info: https://ckarduino.wordpress.com/2022/06/19/iot-knoten-mit-01space-esp32-c3-0-42lcd/
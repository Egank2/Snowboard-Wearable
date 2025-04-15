# software
## Overview
### snowtrack
ESP-IDF project that was supposed to run on the ESP32. After having issues with the newer v5.5.0 I2C driver along with time constraints due to ski resort availability, we decided to abandon this project.

The reason for the issues stems from the BNO08x requiring that the host device has SCL clock stretching enabled. However, the ESP32 I2C driver does not have SCL stretch capability. This was realized after committing to using a ESP32-WROVER and I2C. If I were to do this again I would redesign the board to use SPI instead.
### snowtrack-proto
Simple arduino program that sends the current rotation of the BNO086 to a host through Bluetooth.

This is what was used to record all of the snowboarding runs.
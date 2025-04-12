#include <Arduino.h>
#include <Adafruit_BNO08x.h>

#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#define BNO08X_CS 10
#define BNO08X_INT 9
#define BNO08X_RESET -1

BluetoothSerial SerialBT;
Adafruit_BNO08x bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;
sh2_SensorId_t reportType = SH2_ROTATION_VECTOR;
sh2_RotationVectorWAcc_t* meas;
long reportIntervalUs = 5000;

// Set bno report type and interval
void setReports(sh2_SensorId_t reportType, long report_interval) {
  Serial.println("Setting desired reports");
  if (!bno08x.enableReport(reportType, report_interval)) {
    Serial.println("Could not enable stabilized remote vector");
  }
}

void setup(void) {
  Serial.begin(115200);

  while (!Serial) delay(10);
  // Start SnowTrack bluetooth server
  SerialBT.begin("SnowTrack");

  Serial.println("Adafruit BNO08x test!");

  // Initialize BNO086 on I2C bus
  if (!bno08x.begin_I2C()) {
    Serial.println("Failed to find BNO08x chip");
    while (1) { delay(10); }
  }
  Serial.println("BNO08x Found!");

  setReports(reportType, reportIntervalUs);

  Serial.println("Reading events");
  delay(100);
}

void loop() {
  if (bno08x.wasReset()) {
    Serial.print("sensor was reset ");
    setReports(reportType, reportIntervalUs);
  }

  // Read sensor event and send to bluetooth clients
  if (bno08x.getSensorEvent(&sensorValue)) {
    if (sensorValue.sensorId == SH2_ROTATION_VECTOR) {
      meas = &sensorValue.un.rotationVector;
      Serial.printf("%fw %fx %fy %fz\n", meas->real, meas->i, meas->j, meas->k);
      SerialBT.printf("%f %f %f %f\n", meas->real, meas->i, meas->j, meas->k);
    }
  }
}

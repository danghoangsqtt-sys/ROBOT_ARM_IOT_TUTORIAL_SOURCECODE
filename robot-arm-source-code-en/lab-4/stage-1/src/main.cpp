// ============================================================
// LAB 4 - STAGE 1: I2C Scanner (I2C Scanner)
// Description: Scan I2C bus to find connected devices.
//        Expected result: Found PCA9685 at address 0x40.
// Hardware: ESP32 DevKit V1, PCA9685
// SDA = GPIO21, SCL = GPIO22
// ============================================================

// === STAGE 1: I2C Scanner ===

#include <Wire.h>

// ------------------------------------------------------------
// Ham setup(): Initialize Serial va I2C
// - Serial 115200 baud to communicate with computer
// - Wire.begin(SDA, SCL) Initialize giao thuc I2C tren ESP32
// ------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=== I2C Scanner ===");
  Serial.println("Scanning I2C bus...");

  // Initialize I2C voi chan SDA=21, SCL=22
  Wire.begin(21, 22);
}

// ------------------------------------------------------------
// Ham loop(): Scan I2C address from 1 to 126
// - Valid I2C address from 1 (0x01) to 126 (0x7E)
// - Address 0 is broadcast, 127 is reserved
// - Wire.beginTransmission(addr): begin sending address
// - Wire.endTransmission(): end transmission and check response
//   + Returns 0: device responded (ACK)
//   + Returns != 0: no device at this address
// ------------------------------------------------------------
void loop() {
  int device_count = 0;

  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    uint8_t error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at 0x");
      if (addr < 16) Serial.print("0");
      Serial.println(addr, HEX);
      device_count++;
    }
  }

  if (device_count == 0) {
    Serial.println("No I2C devices found!");
  } else {
    Serial.print("Found ");
    Serial.print(device_count);
    Serial.println(" devices.");
  }

  Serial.println("--- Rescanning in 5 seconds ---");
  delay(5000);
}

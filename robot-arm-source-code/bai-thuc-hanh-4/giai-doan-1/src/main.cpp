// ============================================================
// BAI THUC HANH 4 - GIAI DOAN 1: QUET I2C (I2C Scanner)
// Mo ta: Quet bus I2C de tim cac thiet bi ket noi.
//        Ket qua mong doi: Tim thay PCA9685 tai dia chi 0x40.
// Phan cung: ESP32 DevKit V1, PCA9685
// SDA = GPIO21, SCL = GPIO22
// ============================================================

// === GIAI DOAN 1: Quet I2C ===

#include <Wire.h>

// ------------------------------------------------------------
// Ham setup(): Khoi tao Serial va I2C
// - Serial 115200 baud de giao tiep voi may tinh
// - Wire.begin(SDA, SCL) khoi tao giao thuc I2C tren ESP32
// ------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=== I2C Scanner ===");
  Serial.println("Dang quet bus I2C...");

  // Khoi tao I2C voi chan SDA=21, SCL=22
  Wire.begin(21, 22);
}

// ------------------------------------------------------------
// Ham loop(): Quet dia chi I2C tu 1 den 126
// - Dia chi I2C hop le tu 1 (0x01) den 126 (0x7E)
// - Dia chi 0 la dia chi broadcast, 127 la dia chi du phong
// - Wire.beginTransmission(addr): bat dau gui dia chi
// - Wire.endTransmission(): ket thuc va kiem tra phan hoi
//   + Tra ve 0: co thiet bi phan hoi (ACK)
//   + Tra ve != 0: khong co thiet bi tai dia chi nay
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
    Serial.println("Khong tim thay thiet bi I2C nao!");
  } else {
    Serial.print("Tim thay ");
    Serial.print(device_count);
    Serial.println(" thiet bi.");
  }

  Serial.println("--- Quet lai sau 5 giay ---");
  delay(5000);
}

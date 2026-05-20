// ============================================================
// LAB 4 - STAGE 2: CHUONG TRINH CHINH
// Description: Dieu khien Robot Arm 6DOF qua Serial.
// Hardware: ESP32 + PCA9685 + 6 Servo
// Giao thuc: Lenh text qua Serial 115200 baud
// ============================================================

// === STAGE 2: Chuong trinh chinh ===

#include <Wire.h>
#include "config.h"
#include "pca9685_driver.h"
#include "servo_ctrl.h"
#include "cmd_parser.h"

// ------------------------------------------------------------
// Ham setup(): Initialize he thong
// - Serial 115200 baud
// - I2C voi SDA=21, SCL=22
// - PCA9685 va servo ve vi tri home
// ------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(500);
  Wire.begin(SDA_PIN, SCL_PIN);
  pca9685_init();
  servo_init();
  Serial.println("ARM INIT");
  Serial.println("ARM READY");
}

// ------------------------------------------------------------
// Ham loop(): Read command from Serial va xu ly
// - Doc chuoi den ky tu xuong dong '\n'
// - Remove extra whitespace
// - Goi cmd_parse() neu co du lieu
// ------------------------------------------------------------
void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input.length() > 0) {
      cmd_parse(input);
    }
  }
}

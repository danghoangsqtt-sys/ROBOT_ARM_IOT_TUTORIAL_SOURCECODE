// ============================================================
// BAI THUC HANH 4 - GIAI DOAN 3: PCA9685 DRIVER
// Mo ta: Dieu khien module PCA9685 qua I2C.
//        Chuyen doi goc sang xung PWM va xuat ra kenh servo.
// ============================================================

// === GIAI DOAN 3: PCA9685 Driver ===

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "config.h"
#include "pca9685_driver.h"

// Tao doi tuong PCA9685 tai dia chi I2C 0x40
Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver(0x40);

// ------------------------------------------------------------
// Ham pca9685_init(): Khoi tao PCA9685
// - Goi pca.begin() de bat dau giao tiep I2C
// - Dat tan so PWM = 50Hz (chu ky 20ms cho servo)
// ------------------------------------------------------------
void pca9685_init() {
  pca.begin();
  pca.setPWMFreq(PWM_FREQ);
  delay(10);
  Serial.println("PCA9685 initialized at 0x40");
}

// ------------------------------------------------------------
// Ham angleToPulse(): Chuyen goc (do) sang gia tri xung PWM
// - Su dung ham map() de anh xa tuyen tinh
// - 0 do  -> SERVOMIN (102) -> xung 0.5ms
// - 180 do -> SERVOMAX (512) -> xung 2.5ms
// ------------------------------------------------------------
int angleToPulse(int angle) {
  return map(angle, 0, 180, SERVOMIN, SERVOMAX);
}

// ------------------------------------------------------------
// Ham pca9685_setAngle(): Dat goc cho mot kenh servo
// - Tinh gia tri pulse tu goc
// - Goi pca.setPWM(channel, 0, pulse) de xuat xung
// ------------------------------------------------------------
void pca9685_setAngle(uint8_t channel, int angle) {
  int pulse = angleToPulse(angle);
  pca.setPWM(channel, 0, pulse);
}

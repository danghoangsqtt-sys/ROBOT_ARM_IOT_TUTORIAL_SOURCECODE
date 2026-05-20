// ============================================================
// LAB 4 - STAGE 2: PCA9685 DRIVER
// Description: Control PCA9685 module via I2C.
//        Convert angle to PWM pulse and output to servo channel.
// ============================================================

// === STAGE 2: PCA9685 Driver ===

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "config.h"
#include "pca9685_driver.h"

// Create PCA9685 object at I2C address 0x40
Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver(0x40);

// ------------------------------------------------------------
// Ham pca9685_init(): Initialize PCA9685
// - Call pca.begin() to start I2C communication
// - Set PWM frequency = 50Hz (20ms cycle for servo)
// ------------------------------------------------------------
void pca9685_init() {
  pca.begin();
  pca.setPWMFreq(PWM_FREQ);
  delay(10);
  Serial.println("PCA9685 initialized at 0x40");
}

// ------------------------------------------------------------
// Ham angleToPulse(): Convert angle (degrees) to PWM pulse value
// - Use map() function for linear mapping
// - 0 do  -> SERVOMIN (102) -> pulse 0.5ms
// - 180 do -> SERVOMAX (512) -> pulse 2.5ms
// ------------------------------------------------------------
int angleToPulse(int angle) {
  return map(angle, 0, 180, SERVOMIN, SERVOMAX);
}

// ------------------------------------------------------------
// Ham pca9685_setAngle(): Set angle for one servo channel
// - Calculate pulse value from angle
// - Call pca.setPWM(channel, 0, pulse) to output pulse
// - Print information to Serial for debugging
// ------------------------------------------------------------
void pca9685_setAngle(uint8_t channel, int angle) {
  int pulse = angleToPulse(angle);
  pca.setPWM(channel, 0, pulse);
}

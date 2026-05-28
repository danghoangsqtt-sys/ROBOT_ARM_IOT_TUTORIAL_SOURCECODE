// ============================================================
// BAI THUC HANH 4 - GIAI DOAN 3: CAU HINH ROBOT ARM 6DOF
// Mo ta: Dinh nghia cau hinh phan cung va tham so servo.
//        Bo sung truong speed cho moi khop.
// ============================================================

// === GIAI DOAN 3: Cau hinh Robot Arm ===

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ------------------------------------------------------------
// Cau truc luu thong tin cau hinh cho moi khop (joint)
// - channel: kenh PWM tren PCA9685 (0-15)
// - minAngle, maxAngle: gioi han goc quay (do)
// - homeAngle: goc ve vi tri home
// - name: ten khop de hien thi
// - speed: toc do mac dinh (1-10) cho khop nay
// ------------------------------------------------------------
struct JointConfig {
  uint8_t channel;
  int minAngle;
  int maxAngle;
  int homeAngle;
  const char* name;
  int speed;
};

// Cau hinh 6 khop cua Robot Arm (bo sung speed)
const JointConfig joints[6] = {
  {0,  0,  180, 90, "Base",        5},
  {1,  70, 150, 70, "Shoulder",    5},
  {2,  0,  150, 90, "Elbow",       5},
  {3,  0,  180, 90, "Wrist Pitch", 5},
  {4,  0,  180, 90, "Wrist Roll",  5},
  {5,  60, 120, 90, "Gripper",     5}
};

// So luong khop
const int NUM_JOINTS = 6;

// ------------------------------------------------------------
// Tham so PWM cho PCA9685
// - SERVOMIN = 102: xung 0.5ms / 20ms * 4096 = 102.4 ~ 102
//   Tuong ung goc 0 do cua servo
// - SERVOMAX = 512: xung 2.5ms / 20ms * 4096 = 512
//   Tuong ung goc 180 do cua servo
// - PWM_FREQ = 50Hz: tan so chuan cho servo (chu ky 20ms)
// ------------------------------------------------------------
const int SERVOMIN = 102;
const int SERVOMAX = 512;
const int PWM_FREQ = 50;

// Chan I2C cua ESP32
const int SDA_PIN = 21;
const int SCL_PIN = 22;

// Toc do mac dinh (1-10), dung de tinh delay giua cac buoc
const int DEFAULT_SPEED = 5;

#endif

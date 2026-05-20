// ============================================================
// BAI THUC HANH 4 - GIAI DOAN 2: SERVO CONTROL
// Mo ta: Dieu khien servo robot arm 6DOF.
//        Di chuyen tu tu, gioi han goc, ve home.
// ============================================================

// === GIAI DOAN 2: Servo Control ===

#include "config.h"
#include "pca9685_driver.h"
#include "servo_ctrl.h"

// Mang luu goc hien tai cua tung khop
int currentAngles[NUM_JOINTS];

// ------------------------------------------------------------
// Ham servo_init(): Khoi tao tat ca servo ve vi tri home
// - Duyet qua 6 khop, dat goc = homeAngle
// - Goi pca9685_setAngle() de xuat xung PWM
// ------------------------------------------------------------
void servo_init() {
  for (int i = 0; i < NUM_JOINTS; i++) {
    currentAngles[i] = joints[i].homeAngle;
    pca9685_setAngle(joints[i].channel, currentAngles[i]);
    delay(100);
  }
}

// ------------------------------------------------------------
// Ham clampAngle(): Gioi han goc trong pham vi cho phep
// - Dung ham constrain() cua Arduino
// ------------------------------------------------------------
int clampAngle(int joint, int angle) {
  return constrain(angle, joints[joint].minAngle, joints[joint].maxAngle);
}

// ------------------------------------------------------------
// Ham servo_move(): Di chuyen khop den goc moi
// - Di chuyen tung buoc 1 do de servo chay muot
// - Delay giua cac buoc = 20 / DEFAULT_SPEED (ms)
// ------------------------------------------------------------
void servo_move(int joint, int targetAngle) {
  targetAngle = clampAngle(joint, targetAngle);
  int current = currentAngles[joint];
  int step = (targetAngle > current) ? 1 : -1;
  int delayMs = 20 / DEFAULT_SPEED;

  while (current != targetAngle) {
    current += step;
    pca9685_setAngle(joints[joint].channel, current);
    delay(delayMs);
  }
  currentAngles[joint] = targetAngle;
}

// ------------------------------------------------------------
// Ham servo_home(): Dua khop ve vi tri home
// - joint == -1: dua tat ca khop ve home
// - joint >= 0: dua khop chi dinh ve home
// ------------------------------------------------------------
void servo_home(int joint) {
  if (joint == -1) {
    for (int i = 0; i < NUM_JOINTS; i++) {
      servo_move(i, joints[i].homeAngle);
    }
  } else {
    servo_move(joint, joints[joint].homeAngle);
  }
}

// ------------------------------------------------------------
// Ham servo_getAngle(): Tra ve goc hien tai cua khop
// ------------------------------------------------------------
int servo_getAngle(int joint) {
  return currentAngles[joint];
}

// ------------------------------------------------------------
// Ham servo_printStatus(): In trang thai tat ca khop
// - Dinh dang: STA:a0,a1,a2,a3,a4,a5
// ------------------------------------------------------------
void servo_printStatus() {
  Serial.print("STA:");
  for (int i = 0; i < NUM_JOINTS; i++) {
    Serial.print(currentAngles[i]);
    if (i < NUM_JOINTS - 1) Serial.print(",");
  }
  Serial.println();
}

// ------------------------------------------------------------
// Ham servo_printInfo(): In bang thong tin cau hinh cac khop
// - Hien thi: ten, kenh, goc min/max, home, goc hien tai
// ------------------------------------------------------------
void servo_printInfo() {
  Serial.println("=== ROBOT ARM 6DOF INFO ===");
  Serial.println("Joint | CH | Min | Max | Home | Current");
  Serial.println("------|----|----|------|------|--------");
  for (int i = 0; i < NUM_JOINTS; i++) {
    Serial.print("J"); Serial.print(i);
    Serial.print(" "); Serial.print(joints[i].name);
    Serial.print(" | "); Serial.print(joints[i].channel);
    Serial.print("  | "); Serial.print(joints[i].minAngle);
    Serial.print("  | "); Serial.print(joints[i].maxAngle);
    Serial.print("  | "); Serial.print(joints[i].homeAngle);
    Serial.print("  | "); Serial.println(currentAngles[i]);
  }
}

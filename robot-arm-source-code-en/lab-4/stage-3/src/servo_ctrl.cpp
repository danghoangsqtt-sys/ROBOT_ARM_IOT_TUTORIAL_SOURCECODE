// ============================================================
// LAB 4 - STAGE 3: SERVO CONTROL
// Description: Dieu khien nang cao Robot Arm 6DOF.
//        Bo sung: toc do rieng, di chuyen dong thoi, cho.
// ============================================================

// === STAGE 3: Servo Control ===

#include "config.h"
#include "pca9685_driver.h"
#include "servo_ctrl.h"

// Mang luu goc hien tai cua tung khop
int currentAngles[NUM_JOINTS];

// Mang luu toc do rieng cua tung khop (1-10)
int jointSpeed[NUM_JOINTS];

// Co Moving state
bool isMoving = false;

// ------------------------------------------------------------
// Ham servo_init(): Initialize servo Return to Home, gan toc do tu config
// ------------------------------------------------------------
void servo_init() {
  for (int i = 0; i < NUM_JOINTS; i++) {
    currentAngles[i] = joints[i].homeAngle;
    jointSpeed[i] = joints[i].speed;
    pca9685_setAngle(joints[i].channel, currentAngles[i]);
    delay(100);
  }
}

// ------------------------------------------------------------
// Ham clampAngle(): Gioi han goc trong pham vi cho phep
// ------------------------------------------------------------
int clampAngle(int joint, int angle) {
  return constrain(angle, joints[joint].minAngle, joints[joint].maxAngle);
}

// ------------------------------------------------------------
// Ham servo_move(): Di chuyen 1 khop, dung jointSpeed rieng
// ------------------------------------------------------------
void servo_move(int joint, int targetAngle) {
  targetAngle = clampAngle(joint, targetAngle);
  int current = currentAngles[joint];
  int step = (targetAngle > current) ? 1 : -1;
  int delayMs = 20 / jointSpeed[joint];
  isMoving = true;
  while (current != targetAngle) {
    current += step;
    pca9685_setAngle(joints[joint].channel, current);
    delay(delayMs);
  }
  currentAngles[joint] = targetAngle;
  isMoving = false;
}

// ------------------------------------------------------------
// Ham servo_home(): Dua khop Return to Home (-1 = tat ca)
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

int servo_getAngle(int joint) { return currentAngles[joint]; }

// ------------------------------------------------------------
// Ham servo_printStatus(): In STA:a0,a1,a2,a3,a4,a5
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
// Ham servo_printInfo(): In bang thong tin cau hinh
// ------------------------------------------------------------
void servo_printInfo() {
  Serial.println("=== ROBOT ARM 6DOF INFO ===");
  Serial.println("Joint | CH | Min | Max | Home | Spd | Cur");
  Serial.println("------|----|-----|-----|------|-----|----");
  for (int i = 0; i < NUM_JOINTS; i++) {
    Serial.print("J"); Serial.print(i);
    Serial.print(" "); Serial.print(joints[i].name);
    Serial.print(" | "); Serial.print(joints[i].channel);
    Serial.print("  | "); Serial.print(joints[i].minAngle);
    Serial.print("  | "); Serial.print(joints[i].maxAngle);
    Serial.print("  | "); Serial.print(joints[i].homeAngle);
    Serial.print("  | "); Serial.print(jointSpeed[i]);
    Serial.print("  | "); Serial.println(currentAngles[i]);
  }
}

// ------------------------------------------------------------
// Ham servo_setSpeed(): Set speed for 1 joint (1-10)
// ------------------------------------------------------------
void servo_setSpeed(int joint, int speed) {
  jointSpeed[joint] = constrain(speed, 1, 10);
}

// ------------------------------------------------------------
// Ham servo_moveAll(): Di chuyen 6 khop dong thoi
// - Tinh so buoc lon nhat (maxSteps)
// - Noi suy tuyen tinh tung buoc cho moi khop
// - Tat ca khop den dich cung luc
// ------------------------------------------------------------
void servo_moveAll(int angles[6]) {
  int targets[NUM_JOINTS];
  int maxSteps = 0;
  for (int i = 0; i < NUM_JOINTS; i++) {
    targets[i] = clampAngle(i, angles[i]);
    int diff = abs(targets[i] - currentAngles[i]);
    if (diff > maxSteps) maxSteps = diff;
  }
  if (maxSteps == 0) return;
  isMoving = true;
  int delayMs = 20 / DEFAULT_SPEED;
  for (int s = 1; s <= maxSteps; s++) {
    for (int i = 0; i < NUM_JOINTS; i++) {
      int newAngle = map(s, 0, maxSteps, currentAngles[i], targets[i]);
      pca9685_setAngle(joints[i].channel, newAngle);
    }
    delay(delayMs);
  }
  for (int i = 0; i < NUM_JOINTS; i++) {
    currentAngles[i] = targets[i];
  }
  isMoving = false;
}

// ------------------------------------------------------------
// Ham servo_wait(): Cho den khi hoan thanh, in "DONE"
// ------------------------------------------------------------
void servo_wait() {
  while (isMoving) { delay(10); }
  Serial.println("DONE");
}

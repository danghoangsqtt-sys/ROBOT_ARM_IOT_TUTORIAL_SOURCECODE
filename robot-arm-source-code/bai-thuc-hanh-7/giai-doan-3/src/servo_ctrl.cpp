// ============================================================
// BAI THUC HANH 7 - GIAI DOAN 2: SERVO CONTROL IMPLEMENTATION
// ============================================================

#include "servo_ctrl.h"
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// --- Tham so PWM cho PCA9685 ---
// SERVOMIN = 102: 0.5ms / 20ms * 4096 = 102.4 → goc 0 do
// SERVOMAX = 512: 2.5ms / 20ms * 4096 = 512.0 → goc 180 do
static const int SERVOMIN = 102;
static const int SERVOMAX = 512;

// Doi tuong PCA9685 (dia chi mac dinh 0x40)
static Adafruit_PWMServoDriver pca9685(0x40);

// Mang luu goc hien tai cua 6 khop (bien toan cuc)
int currentAngles[6];

// ============================================================
// _servoWrite() - Ham noi bo ghi xung PWM den PCA9685
// Input:  channel (0-5), angle (0-180)
// Logic:  map(angle, 0, 180, SERVOMIN, SERVOMAX)
// ============================================================
static void _servoWrite(uint8_t channel, int angle) {
  int pulse = map(angle, 0, 180, SERVOMIN, SERVOMAX);
  pca9685.setPWM(channel, 0, pulse);
}

// ============================================================
// servo_init() - Khoi tao PCA9685 va dua tat ca servo ve HOME
// ============================================================
void servo_init() {
  Wire.begin(21, 22);             // SDA=21, SCL=22
  pca9685.begin();
  pca9685.setOscillatorFrequency(27000000);
  pca9685.setPWMFreq(50);         // 50 Hz: chuan RC servo
  delay(10);

  // Dua tat ca khop ve vi tri HOME
  for (int i = 0; i < NUM_JOINTS; i++) {
    currentAngles[i] = JOINTS[i].homeAngle;
    _servoWrite(JOINTS[i].channel, JOINTS[i].homeAngle);
    delay(50);  // Delay nho giua cac servo tranh tang dong
  }
  Serial.println("[Servo] Tat ca khop da ve HOME");
}

// ============================================================
// servo_move() - Di chuyen 1 khop den goc chi dinh
// Input:  joint (0-5), angle (do)
// Return: goc thuc te sau khi clamp vao [min, max]
// ============================================================
int servo_move(int joint, int angle) {
  if (joint < 0 || joint >= NUM_JOINTS) return -1;

  // Gioi han goc trong pham vi hop le cua khop
  angle = constrain(angle, JOINTS[joint].minAngle, JOINTS[joint].maxAngle);
  _servoWrite(JOINTS[joint].channel, angle);
  currentAngles[joint] = angle;

  Serial.printf("[Servo] Khop %d (%s) → %d do\n",
                joint, JOINTS[joint].name, angle);
  return angle;
}

// ============================================================
// servo_home() - Dua khop ve vi tri mac dinh
// joint = -1 → tat ca khop; joint = 0..5 → 1 khop
// ============================================================
void servo_home(int joint) {
  if (joint == -1) {
    // Tat ca khop
    for (int i = 0; i < NUM_JOINTS; i++) {
      _servoWrite(JOINTS[i].channel, JOINTS[i].homeAngle);
      currentAngles[i] = JOINTS[i].homeAngle;
      delay(50);
    }
    Serial.println("[Servo] Tat ca khop ve HOME");
  } else if (joint >= 0 && joint < NUM_JOINTS) {
    _servoWrite(JOINTS[joint].channel, JOINTS[joint].homeAngle);
    currentAngles[joint] = JOINTS[joint].homeAngle;
    Serial.printf("[Servo] Khop %d ve HOME (%d do)\n",
                  joint, JOINTS[joint].homeAngle);
  }
}

// ============================================================
// servo_moveAll() - Di chuyen dong thoi 6 khop
// Input: mang angles[6] chua goc moi cho tung khop
// ============================================================
void servo_moveAll(int angles[6]) {
  for (int i = 0; i < NUM_JOINTS; i++) {
    int angle = constrain(angles[i], JOINTS[i].minAngle, JOINTS[i].maxAngle);
    _servoWrite(JOINTS[i].channel, angle);
    currentAngles[i] = angle;
  }
  Serial.println("[Servo] MoveAll hoan thanh");
}

// ============================================================
// servo_getStatusString() - Tao chuoi trang thai "a0,a1,...,a5"
// Dung cho lenh T (Get Status) tra ve client
// ============================================================
String servo_getStatusString() {
  String s = "STA:";
  for (int i = 0; i < NUM_JOINTS; i++) {
    s += String(currentAngles[i]);
    if (i < NUM_JOINTS - 1) s += ",";
  }
  return s;
}

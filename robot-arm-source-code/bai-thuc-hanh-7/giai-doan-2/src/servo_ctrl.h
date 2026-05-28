// ============================================================
// BAI THUC HANH 7 - GIAI DOAN 2: SERVO CONTROL HEADER
// Mo ta: Khai bao cac ham dieu khien 6 servo qua PCA9685.
//        Copy tu BTH 4 Giai Doan 3, chinh sua nhe de phu hop
//        voi BTH 7 (khong co Serial CLI, bo sung getAnglesJSON).
// ============================================================

#ifndef SERVO_CTRL_H
#define SERVO_CTRL_H

#include <Arduino.h>

// -----------------------------------------------------------
// Cau truc cau hinh moi khop (giong BTH 4 Giai Doan 3)
// -----------------------------------------------------------
struct JointConfig {
  uint8_t    channel;   // Kenh PCA9685 (0-15)
  int        minAngle;  // Goc toi thieu (do)
  int        maxAngle;  // Goc toi da (do)
  int        homeAngle; // Vi tri mac dinh
  const char* name;     // Ten khop (de hien thi)
};

// Cau hinh 6 khop (giong BTH 4 Giai Doan 3)
const JointConfig JOINTS[6] = {
  {0,  0,  180, 90, "Base"},
  {1, 70,  150, 70, "Shoulder"},
  {2,  0,  150, 90, "Elbow"},
  {3,  0,  180, 90, "Wrist Pitch"},
  {4,  0,  180, 90, "Wrist Roll"},
  {5, 60,  120, 90, "Gripper"}
};
const int NUM_JOINTS = 6;

// Mang luu goc hien tai cua 6 khop
extern int currentAngles[6];

// Khoi tao PCA9685 va dua tat ca servo ve HOME
void servo_init();

// Di chuyen 1 khop den goc chi dinh
// Tra ve goc thuc te sau khi ap dung gioi han (clamp)
int servo_move(int joint, int angle);

// Dua 1 hoac tat ca khop ve HOME
// joint = -1 → tat ca khop; joint = 0..5 → 1 khop cu the
void servo_home(int joint);

// Di chuyen dong thoi 6 khop (goc truyen vao mang angles[6])
void servo_moveAll(int angles[6]);

// Lay chuoi trang thai dang "a0,a1,a2,a3,a4,a5"
// (dung cho lenh T - Get Status)
String servo_getStatusString();

#endif

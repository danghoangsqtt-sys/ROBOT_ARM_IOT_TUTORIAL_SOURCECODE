// Servo ctrl header - copy tu stage-2, giong hoan toan
// (STAGE 3 dung lai servo_ctrl khong thay doi)
// ============================================================
// LAB 7 - STAGE 3: SERVO CONTROL HEADER
// ============================================================

#ifndef SERVO_CTRL_H
#define SERVO_CTRL_H

#include <Arduino.h>

struct JointConfig {
  uint8_t    channel;
  int        minAngle;
  int        maxAngle;
  int        homeAngle;
  const char* name;
};

const JointConfig JOINTS[6] = {
  {0,  0,  180, 90, "Base"},
  {1, 70,  150, 70, "Shoulder"},
  {2,  0,  150, 90, "Elbow"},
  {3,  0,  180, 90, "Wrist Pitch"},
  {4,  0,  180, 90, "Wrist Roll"},
  {5, 60,  120, 90, "Gripper"}
};
const int NUM_JOINTS = 6;

extern int currentAngles[6];

void servo_init();
int  servo_move(int joint, int angle);
void servo_home(int joint);
void servo_moveAll(int angles[6]);
String servo_getStatusString();

#endif

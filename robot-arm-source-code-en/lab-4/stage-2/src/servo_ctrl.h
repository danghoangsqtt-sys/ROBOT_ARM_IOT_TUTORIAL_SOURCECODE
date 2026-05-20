// ============================================================
// LAB 4 - STAGE 2: SERVO CONTROL HEADER
// Description: Khai bao cac ham dieu khien servo robot arm.
// ============================================================

// === STAGE 2: Servo Control ===

#ifndef SERVO_CTRL_H
#define SERVO_CTRL_H

#include <Arduino.h>

// Array storing current angles of 6 joints
extern int currentAngles[6];

// Initialize tat ca servo ve vi tri home
void servo_init();

// Gioi han goc trong pham vi cho phep cua khop
int clampAngle(int joint, int angle);

// Di chuyen khop den goc chi dinh (di chuyen tu tu 1 do/buoc)
void servo_move(int joint, int targetAngle);

// Dua khop ve vi tri home (-1 = tat ca khop)
void servo_home(int joint);

// Lay goc hien tai cua khop
int servo_getAngle(int joint);

// Print status of all joints
void servo_printStatus();

// In bang thong tin cau hinh cac khop
void servo_printInfo();

#endif

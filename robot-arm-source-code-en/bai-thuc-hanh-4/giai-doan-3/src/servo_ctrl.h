// ============================================================
// BAI THUC HANH 4 - GIAI DOAN 3: SERVO CONTROL HEADER
// Mo ta: Khai bao cac ham dieu khien servo robot arm.
//        Bo sung: setSpeed, moveAll, wait.
// ============================================================

// === GIAI DOAN 3: Servo Control ===

#ifndef SERVO_CTRL_H
#define SERVO_CTRL_H

#include <Arduino.h>

// Mang luu goc hien tai cua 6 khop
extern int currentAngles[6];

// Mang luu toc do cua tung khop (1-10)
extern int jointSpeed[6];

// Khoi tao tat ca servo ve vi tri home
void servo_init();

// Gioi han goc trong pham vi cho phep cua khop
int clampAngle(int joint, int angle);

// Di chuyen khop den goc chi dinh (di chuyen tu tu 1 do/buoc)
void servo_move(int joint, int targetAngle);

// Dua khop ve vi tri home (-1 = tat ca khop)
void servo_home(int joint);

// Lay goc hien tai cua khop
int servo_getAngle(int joint);

// In trang thai tat ca cac khop
void servo_printStatus();

// In bang thong tin cau hinh cac khop
void servo_printInfo();

// Dat toc do cho khop (1-10)
void servo_setSpeed(int joint, int speed);

// Di chuyen dong thoi 6 khop (noi suy tuyen tinh)
void servo_moveAll(int angles[6]);

// Cho den khi tat ca servo hoan thanh di chuyen
void servo_wait();

#endif

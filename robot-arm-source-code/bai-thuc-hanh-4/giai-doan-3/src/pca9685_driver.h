// ============================================================
// BAI THUC HANH 4 - GIAI DOAN 3: PCA9685 DRIVER HEADER
// Mo ta: Khai bao cac ham dieu khien PCA9685.
// ============================================================

// === GIAI DOAN 3: PCA9685 Driver ===

#ifndef PCA9685_DRIVER_H
#define PCA9685_DRIVER_H

#include <Arduino.h>

// Khoi tao module PCA9685
void pca9685_init();

// Dat goc cho kenh servo chi dinh
void pca9685_setAngle(uint8_t channel, int angle);

// Chuyen doi goc (0-180) sang gia tri xung PWM
int angleToPulse(int angle);

#endif

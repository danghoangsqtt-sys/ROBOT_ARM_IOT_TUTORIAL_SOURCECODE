// ============================================================
// LAB 4 - STAGE 3: PCA9685 DRIVER HEADER
// Description: Declare functions to control PCA9685.
// ============================================================

// === STAGE 3: PCA9685 Driver ===

#ifndef PCA9685_DRIVER_H
#define PCA9685_DRIVER_H

#include <Arduino.h>

// Initialize module PCA9685
void pca9685_init();

// Set angle for specific servo channel
void pca9685_setAngle(uint8_t channel, int angle);

// Convert angle (0-180) to PWM pulse value
int angleToPulse(int angle);

#endif

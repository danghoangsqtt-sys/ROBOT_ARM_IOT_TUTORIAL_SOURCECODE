// ============================================================
// LAB 4 - STAGE 3: 6DOF ROBOT ARM CONFIGURATION
// Description: Define hardware configuration and servo parameters.
//        Bo sung truong speed cho moi khop.
// ============================================================

// === STAGE 3: Cau hinh Robot Arm ===

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ------------------------------------------------------------
// Structure to store configuration for each joint
// - channel: PWM channel on PCA9685 (0-15)
// - minAngle, maxAngle: rotation angle limit (degrees)
// - homeAngle: angle to return to home position
// - name: joint name for display
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

// Configuration of 6 Robot Arm joints (bo sung speed)
const JointConfig joints[6] = {
  {0,  0,  180, 90, "Base",        5},
  {1,  70, 150, 70, "Shoulder",    5},
  {2,  0,  150, 90, "Elbow",       5},
  {3,  0,  180, 90, "Wrist Pitch", 5},
  {4,  0,  180, 90, "Wrist Roll",  5},
  {5,  60, 120, 90, "Gripper",     5}
};

// Number of joints
const int NUM_JOINTS = 6;

// ------------------------------------------------------------
// PWM parameters for PCA9685
// - SERVOMIN = 102: pulse 0.5ms / 20ms * 4096 = 102.4 ~ 102
//   Corresponding to 0 degrees of servo
// - SERVOMAX = 512: pulse 2.5ms / 20ms * 4096 = 512
//   Corresponding to 180 degrees of servo
// - PWM_FREQ = 50Hz: standard frequency for servo (20ms cycle)
// ------------------------------------------------------------
const int SERVOMIN = 102;
const int SERVOMAX = 512;
const int PWM_FREQ = 50;

// I2C pins of ESP32
const int SDA_PIN = 21;
const int SCL_PIN = 22;

// Default speed (1-10), used to calculate delay between steps
const int DEFAULT_SPEED = 5;

#endif

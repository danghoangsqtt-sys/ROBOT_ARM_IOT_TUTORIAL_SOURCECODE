/*=========================================================================
  LAB 6 - STAGE 1
  Ket noi Blynk IoT co ban - Dieu khien 1 servo (Base) qua WiFi
  
  Hardware: ESP32 DevKit V1 + PCA9685 + Servo MG996R
  Giao tiep: I2C (SDA=GPIO21, SCL=GPIO22), PCA9685 addr=0x40
  Blynk: V0 = Slider dieu khien goc servo Base (0-180 do)
=========================================================================*/

// === STAGE 1: KET NOI BLYNK CO BAN ===

// QUAN TRONG: BLYNK_PRINT phai dat TRUOC tat ca #include
// Neu dat sau -> Serial debug cua Blynk se khong hoat dong
#define BLYNK_PRINT Serial

// Thong tin xac thuc Blynk - lay tu Blynk Console (blynk.cloud)
#define BLYNK_TEMPLATE_ID   "YOUR_TEMPLATE_ID"    // <-- THAY BANG TEMPLATE ID TU BLYNK
#define BLYNK_TEMPLATE_NAME "Robot Arm IoT"
#define BLYNK_AUTH_TOKEN    "YOUR_TOKEN"            // <-- REPLACE WITH BLYNK AUTH TOKEN

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Thong tin WiFi - thay bang WiFi thuc te cua ban
char ssid[] = "TEN_WIFI_CUA_BAN";                  // <-- REPLACE WITH ACTUAL WIFI NAME
char pass[] = "MAT_KHAU_WIFI";                      // <-- REPLACE WITH WIFI PASSWORD

// Initialize PCA9685 tai dia chi I2C 0x40
Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver(0x40);

// Cau hinh PWM cho servo: 0.5ms=0do, 2.5ms=180do (tan so 50Hz, 12-bit)
#define SERVOMIN 102   // Xung PWM tuong ung 0 do   (0.5ms / 20ms * 4096)
#define SERVOMAX 512   // Xung PWM tuong ung 180 do  (2.5ms / 20ms * 4096)

/*-----------------------------------------------------------------------
  angleToPulse: Convert angle (degrees) to PWM pulse value 12-bit
  - angle: goc can chuyen (0-180)
  - return: gia tri pulse (102-512)
-----------------------------------------------------------------------*/
int angleToPulse(int angle) {
  return map(angle, 0, 180, SERVOMIN, SERVOMAX);
}

/*-----------------------------------------------------------------------
  moveServo: Dieu khien 1 kenh servo tren PCA9685
  - ch: kenh PCA9685 (0-15)
  - angle: goc mong muon (0-180 do)
-----------------------------------------------------------------------*/
void moveServo(uint8_t ch, int angle) {
  int pulse = angleToPulse(angle);
  pca.setPWM(ch, 0, pulse);
  Serial.print("Servo CH");
  Serial.print(ch);
  Serial.print(" -> ");
  Serial.print(angle);
  Serial.println(" do");
}

/*-----------------------------------------------------------------------
  BLYNK_WRITE(V0): Callback tu Blynk khi nguoi dung keo Slider V0
  Luong du lieu: Dashboard -> Blynk Cloud -> WiFi -> ESP32 -> PCA9685 -> Servo
  Day la macro dac biet cua Blynk SDK, tu dong goi khi Virtual Pin thay doi
-----------------------------------------------------------------------*/
BLYNK_WRITE(V0) {
  int angle = param.asInt();  // Doc gia tri tu Slider (0-180)
  moveServo(0, angle);        // Quay servo Base (kenh 0)
}

/*-----------------------------------------------------------------------
  setup: Initialize he thong
-----------------------------------------------------------------------*/
void setup() {
  Serial.begin(115200);
  Serial.println("=== BAI 6 - GD1: Blynk co ban ===");

  // Initialize I2C voi chan SDA=21, SCL=22
  Wire.begin(21, 22);

  // Initialize PCA9685 va dat tan so PWM 50Hz cho servo
  pca.begin();
  pca.setPWMFreq(50);

  // Dua servo Base ve vi tri home (90 do)
  moveServo(0, 90);

  // Ket noi Blynk - ham nay tu dong ket noi WiFi va Blynk Cloud
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("Blynk da ket noi thanh cong!");
}

/*-----------------------------------------------------------------------
  loop: Vong lap chinh - chi can goi Blynk.run()
  Blynk.run() xu ly: nhan lenh tu Cloud, gui du lieu len, duy tri ket noi
-----------------------------------------------------------------------*/
void loop() {
  Blynk.run();
}

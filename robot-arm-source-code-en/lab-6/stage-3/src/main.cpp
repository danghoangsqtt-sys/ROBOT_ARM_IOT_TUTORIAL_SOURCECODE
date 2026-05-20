/*=========================================================================
  LAB 6 - STAGE 3
  COMPLETE IOT SYSTEM - Web Dashboard + Mobile App
  Hardware: ESP32 + PCA9685 + 6 Servo | V0-V5, V6(Home), V7(Status)
=========================================================================*/

// === STAGE 3: COMPLETE IOT SYSTEM ===
//
// This firmware is EXACTLY THE SAME as STAGE 2.
// BLYNK SDK has built-in:
//   1. Supports both Web Dashboard and Mobile App (cung 1 firmware)
//   2. Auto reconnect when WiFi is lost (Blynk.run() xu ly ben trong)
//   3. Real-time status sync between Web and Mobile
//
// TESTING INSTRUCTIONS:
//   - Install BLYNK IoT app on mobile phone (iOS/Android)
//   - Login with created BLYNK account
//   - Open Device "Robot Arm IoT" tren app
//   - Drag Slider on phone -> robot responds
//   - Turn off WiFi router for 10s -> turn on -> ESP32 auto reconnects
//   - Open both Web + Mobile at the same time -> 2-way sync

#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID   "YOUR_TEMPLATE_ID"    // <-- THAY BANG TEMPLATE ID TU BLYNK
#define BLYNK_TEMPLATE_NAME "Robot Arm IoT"
#define BLYNK_AUTH_TOKEN    "YOUR_TOKEN"            // <-- REPLACE WITH BLYNK AUTH TOKEN

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

char ssid[] = "TEN_WIFI_CUA_BAN";                  // <-- REPLACE WITH ACTUAL WIFI NAME
char pass[] = "MAT_KHAU_WIFI";                      // <-- REPLACE WITH WIFI PASSWORD

Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver(0x40);

#define SERVOMIN 102
#define SERVOMAX 512

int home_angles[6]    = {90, 70, 90, 90, 90, 90};
int current_angles[6] = {90, 70, 90, 90, 90, 90};

BlynkTimer timer;

int angleToPulse(int angle) {
  return map(angle, 0, 180, SERVOMIN, SERVOMAX);
}

/*--- moveServo: Dieu khien servo theo kenh va goc ---*/
void moveServo(uint8_t ch, int angle) {
  int pulse = angleToPulse(angle);
  pca.setPWM(ch, 0, pulse);
  Serial.print("J");
  Serial.print(ch);
  Serial.print("=");
  Serial.println(angle);
}

// === BLYNK_WRITE: Callback khi nguoi dung thay doi Slider tren Dashboard ===
BLYNK_WRITE(V0) { int a = param.asInt(); moveServo(0, a); current_angles[0] = a; }
BLYNK_WRITE(V1) { int a = param.asInt(); moveServo(1, a); current_angles[1] = a; }
BLYNK_WRITE(V2) { int a = param.asInt(); moveServo(2, a); current_angles[2] = a; }
BLYNK_WRITE(V3) { int a = param.asInt(); moveServo(3, a); current_angles[3] = a; }
BLYNK_WRITE(V4) { int a = param.asInt(); moveServo(4, a); current_angles[4] = a; }
BLYNK_WRITE(V5) { int a = param.asInt(); moveServo(5, a); current_angles[5] = a; }

/*--- BLYNK_WRITE(V6): Nut Home - Dong bo lai Slider tren Dashboard ---*/
BLYNK_WRITE(V6) {
  if (param.asInt() == 1) {
    for (int i = 0; i < 6; i++) {
      moveServo(i, home_angles[i]);
      current_angles[i] = home_angles[i];
    }
    Blynk.virtualWrite(V0, home_angles[0]);
    Blynk.virtualWrite(V1, home_angles[1]);
    Blynk.virtualWrite(V2, home_angles[2]);
    Blynk.virtualWrite(V3, home_angles[3]);
    Blynk.virtualWrite(V4, home_angles[4]);
    Blynk.virtualWrite(V5, home_angles[5]);
    Blynk.virtualWrite(V7, "HOME");
    Serial.println(">>> HOME position <<<");
  }
}

/*--- sendStatus: Gui trang thai goc hien tai len V7 moi 3 giay ---*/
void sendStatus() {
  String status_str = "STA:";
  for (int i = 0; i < 6; i++) {
    status_str += String(current_angles[i]);
    if (i < 5) status_str += ",";
  }
  Blynk.virtualWrite(V7, status_str);
}

void setup() {
  Serial.begin(115200);
  Serial.println("=== BAI 6 - GD3: Hoan thien IoT ===");

  Wire.begin(21, 22);
  pca.begin();
  pca.setPWMFreq(50);

  for (int i = 0; i < 6; i++) {
    moveServo(i, home_angles[i]);
    current_angles[i] = home_angles[i];
  }

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(3000L, sendStatus);
  Serial.println("Blynk da ket noi - san sang dieu khien!");
}

/*--- loop: Blynk.run() + timer.run() - KHONG dung delay() o day ---*/
void loop() {
  Blynk.run();
  timer.run();
}

// ============================================================
// LAB 7 - STAGE 1: CHUONG TRINH CHINH
// Description: ESP32 WiFi Connection (Station Mode), phat Web Server,
//        Receive lenh dieu khien 1 servo (Base) qua WebSocket.
//        Protocol WebSocket: plain-text "M <joint> <angle>"
//        (giong Protocol Serial cua BTH 4-5)
// Hardware: ESP32 DevKit V1 + PCA9685 + Servo Channel 0
// Thu vien: ESPAsyncWebServer, AsyncTCP, Adafruit PCA9685
// ============================================================

// === STAGE 1: WiFi + WebSocket + 1 Servo (PROGMEM) ===

#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <ESPAsyncWebServer.h>          // HTTP Server bat dong bo
#include <Adafruit_PWMServoDriver.h>    // Dieu khien PCA9685

#include "index_html.h"                 // Trang embedded HTML (PROGMEM)

// ============================================================
// THAY DOI 2 DONG NAY THANH THONG TIN WIFI CUA BAN
// ============================================================
const char* WIFI_SSID = "MAI LOAN";
const char* WIFI_PASS = "mailoan272";

// --- Tham so PCA9685 ---
// SERVOMIN = 102: tuong ung 0.5ms / 20ms * 4096 = goc 0 do
// SERVOMAX = 512: tuong ung 2.5ms / 20ms * 4096 = goc 180 do
const int SERVOMIN = 102;
const int SERVOMAX = 512;

// --- Cau hinh Joint 0 (Base) ---
const int JOINT0_MIN  = 0;    // Goc toi thieu (do)
const int JOINT0_MAX  = 180;  // Goc toi da (do)
const int JOINT0_HOME = 90;   // Vi tri mac dinh

// --- Goc hien Load cua servo 0 ---
int currentAngle = JOINT0_HOME;

// --- Doi tuong PCA9685 va Web Server ---
Adafruit_PWMServoDriver pca9685 = Adafruit_PWMServoDriver(0x40);
AsyncWebServer  server(80);   // HTTP server cong 80
AsyncWebSocket  ws("/ws");    // WebSocket Load duong dan /ws

// ============================================================
// servoWrite() - Ghi goc den PCA9685
// Input:  channel (0-15), angle (0-180)
// Logic:  Chuyen doi goc → PWM tick theo cong thuc tuyen tinh
// ============================================================
void servoWrite(uint8_t channel, int angle) {
  // Gioi han goc trong pham vi hop le
  angle = constrain(angle, 0, 180);
  // Chuyen goc → PWM tick: map(angle, 0, 180, SERVOMIN, SERVOMAX)
  int pulse = map(angle, 0, 180, SERVOMIN, SERVOMAX);
  pca9685.setPWM(channel, 0, pulse);
}

// ============================================================
// parseAndExecute() - Xu ly lenh WebSocket plain-text
// Protocol (same as LAB 4-5):
//   "M 0 90" → Di chuyen Joint 0 den goc 90
//   "H"      → Ve vi tri home
//   "T"      → return goc hien Load
// ============================================================
void parseAndExecute(AsyncWebSocketClient* client, const String& cmd) {
  Serial.println("[WS] Receive: " + cmd);

  if (cmd.startsWith("M ")) {
    // --- Lenh di chuyen: "M <joint> <angle>" ---
    // STAGE 1 chi ho tro Joint 0 (mo rong o STAGE 2)
    int spacePos = cmd.indexOf(' ', 2);
    if (spacePos > 0) {
      int joint = cmd.substring(2, spacePos).toInt();
      int angle = cmd.substring(spacePos + 1).toInt();
      if (joint == 0) {
        angle = constrain(angle, JOINT0_MIN, JOINT0_MAX);
        servoWrite(0, angle);
        currentAngle = angle;
        client->text("OK");
        Serial.printf("[Servo] Joint 0 → %d do\n", angle);
      } else {
        client->text("ERR: STAGE 1 chi ho tro Joint 0");
      }
    }

  } else if (cmd == "H") {
    // --- Lenh HOME: ve vi tri mac dinh ---
    servoWrite(0, JOINT0_HOME);
    currentAngle = JOINT0_HOME;
    client->text("OK");
    Serial.println("[Servo] HOME");

  } else if (cmd == "T") {
    // --- Lenh lay trang thai: return goc hien Load ---
    client->text("VAL:0:" + String(currentAngle));

  } else {
    client->text("ERR: Invalid command");
  }
}

// ============================================================
// onWebSocketEvent() - Xu ly cac su kien WebSocket
// ESP32 dong vai tro SERVER, browser dong vai tro CLIENT
// ============================================================
void onWebSocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client,
                      AwsEventType type, void* arg, uint8_t* data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      // Client moi ket noi
      Serial.printf("[WS] Client %u Connected (IP: %s)\n",
                    client->id(), client->remoteIP().toString().c_str());
      client->text("HELLO");  // Send loi chao
      break;

    case WS_EVT_DISCONNECT:
      Serial.printf("[WS] Client %u da ngat ket noi\n", client->id());
      break;

    case WS_EVT_DATA: {
      // Receive du lieu tu client
      AwsFrameInfo* info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len
          && info->opcode == WS_TEXT) {
        // Frame TEXT hoan chinh → xu ly lenh
        String cmd = String((char*)data, len);
        cmd.trim();
        parseAndExecute(client, cmd);
      }
      break;
    }

    case WS_EVT_ERROR:
      Serial.println("[WS] Loi WebSocket");
      break;

    default:
      break;
  }
}

// ============================================================
// setup() - Initialize he thong
// Thu tu: Serial → PCA9685 → WiFi → WebSocket → HTTP Server
// ============================================================
void setup() {
  // 1. Initialize Serial de debug
  Serial.begin(115200);
  delay(500);
  Serial.println("\n=== BTH 7 - STAGE 1: WiFi WebSocket ===");

  // 2. Initialize I2C va PCA9685
  Wire.begin(21, 22);           // SDA=21, SCL=22 (chan I2C cua ESP32)
  pca9685.begin();
  pca9685.setOscillatorFrequency(27000000);
  pca9685.setPWMFreq(50);       // 50 Hz: tan so chuan cho servo
  delay(10);

  // 3. Dua servo ve vi tri HOME
  servoWrite(0, JOINT0_HOME);
  Serial.println("[Servo] Da ve vi tri HOME (90 do)");

  // 4. WiFi Connection (Station Mode)
  Serial.printf("[WiFi] Connecting toi SSID: %s\n", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("[WiFi] Connection successful!");
  Serial.print("[WiFi] IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("[WiFi] Open browser va nhap IP address tren de ket noi!");

  // 5. Configure WebSocket handler
  ws.onEvent(onWebSocketEvent);
  server.addHandler(&ws);

  // 6. Cau hinh HTTP route: GET "/" → return trang embedded HTML
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    // serve trang HTML tu PROGMEM (Flash), khong dung SPIFFS
    request->send_P(200, "text/html", INDEX_HTML);
  });

  // 7. Bat dau HTTP Server
  server.begin();
  Serial.println("[Server] HTTP + WebSocket Server has started");
  Serial.println("[Server] Open browser: http://" + WiFi.localIP().toString());
}

// ============================================================
// loop() - Vong lap chinh
// Nhiem vu: Don dep cac ket noi WebSocket cu (cleanup)
// ESPAsyncWebServer xu ly request bat dong bo (khong can phai
// polling trong loop), nhung can goi cleanupClients() dinh ky
// ============================================================
void loop() {
  // Don dep client WebSocket bi ngat ket noi (chay moi 1 seconds)
  static unsigned long lastCleanup = 0;
  if (millis() - lastCleanup > 1000) {
    ws.cleanupClients();
    lastCleanup = millis();
  }
}

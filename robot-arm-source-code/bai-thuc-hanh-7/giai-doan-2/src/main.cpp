// ============================================================
// BAI THUC HANH 7 - GIAI DOAN 2: CHUONG TRINH CHINH
// Mo ta: ESP32 ket noi WiFi, phuc vu Web GUI 6 khop tu SPIFFS.
//        Diem khac so voi Giai doan 1:
//        - 6 khop (thay vi 1 khop)
//        - File HTML/JS/CSS luu trong SPIFFS (thay vi PROGMEM)
//        - Code tach module: servo_ctrl + ws_handler
// Phan cung: ESP32 DevKit V1 + PCA9685 + 6 Servo MG996R
// ============================================================

// === GIAI DOAN 2: SPIFFS + Web GUI 6 Khop ===

#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>

#include "servo_ctrl.h"
#include "ws_handler.h"

// ============================================================
// THAY DOI 2 DONG NAY THANH THONG TIN WIFI CUA BAN
// ============================================================
const char* WIFI_SSID = "TEN_WIFI_CUA_BAN";
const char* WIFI_PASS = "MAT_KHAU_WIFI";

// Doi tuong server va WebSocket
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// ============================================================
// setup() - Khoi tao theo thu tu:
// Serial → SPIFFS → Servo (PCA9685) → WiFi → WebSocket → Server
// ============================================================
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n=== BTH 7 - GIAI DOAN 2: Web GUI 6 Khop (SPIFFS) ===");

  // 1. Khoi tao SPIFFS (Flash Filesystem)
  // SPIFFS chua cac file HTML/JS/CSS duoc upload tu thu muc data/
  if (!SPIFFS.begin(true)) {
    Serial.println("[SPIFFS] Loi: Khong the khoi tao SPIFFS!");
    Serial.println("[SPIFFS] Hay chay: pio run -t uploadfs");
    // Tiep tuc de debug (server van hoat dong du khong co file web)
  } else {
    Serial.println("[SPIFFS] Khoi tao thanh cong");
    // Liet ke cac file co trong SPIFFS de debug
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    while (file) {
      Serial.printf("[SPIFFS] File: %s (%d bytes)\n", file.name(), file.size());
      file = root.openNextFile();
    }
  }

  // 2. Khoi tao servo (PCA9685) va dua ve HOME
  servo_init();

  // 3. Ket noi WiFi Station Mode
  Serial.printf("[WiFi] Dang ket noi SSID: %s\n", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("[WiFi] Ket noi thanh cong!");
  Serial.print("[WiFi] Dia chi IP: ");
  Serial.println(WiFi.localIP());
  Serial.println(">>> Mo browser: http://" + WiFi.localIP().toString());

  // 4. Cau hinh WebSocket
  ws_setup(ws);
  server.addHandler(&ws);

  // 5. Cau hinh HTTP routes - phuc vu file tu SPIFFS
  // GET "/" → tra ve /index.html tu SPIFFS
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
    req->send(SPIFFS, "/index.html", "text/html");
  });

  // Phuc vu cac file static (JS, CSS) tu SPIFFS tu dong
  // Vi du: GET "/app.js" → SPIFFS /app.js
  server.serveStatic("/", SPIFFS, "/");

  // 6. Xu ly 404 - file khong tim thay
  server.onNotFound([](AsyncWebServerRequest* req) {
    req->send(404, "text/plain", "404: File khong tim thay");
  });

  // 7. Bat dau server
  server.begin();
  Serial.println("[Server] HTTP + WebSocket Server da khoi dong");
}

// ============================================================
// loop() - Don dep WebSocket client bi ngat ket noi
// ============================================================
void loop() {
  static unsigned long lastCleanup = 0;
  if (millis() - lastCleanup > 1000) {
    ws.cleanupClients();
    lastCleanup = millis();
  }
}

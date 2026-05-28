// ============================================================
// LAB 7 - STAGE 2: CHUONG TRINH CHINH
// Description: ESP32 WiFi Connection, serve Web Send 6 Joint tu SPIFFS.
//        Difference from STAGE 1:
//        - 6 Joint (instead of 1 Joint)
//        - File HTML/JS/CSS Save trong SPIFFS (instead of PROGMEM)
//        - Module separated code: servo_ctrl + ws_handler
// Hardware: ESP32 DevKit V1 + PCA9685 + 6 Servo MG996R
// ============================================================

// === STAGE 2: SPIFFS + Web Send 6 Joint ===

#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>

#include "servo_ctrl.h"
#include "ws_handler.h"

// ============================================================
// THAY DOI 2 DONG NAY THANH THONG TIN WIFI CUA BAN
// ============================================================
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";

// Server and WebSocket objects
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// ============================================================
// setup() - Initialize in order:
// Serial → SPIFFS → Servo (PCA9685) → WiFi → WebSocket → Server
// ============================================================
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n=== BTH 7 - STAGE 2: Web Send 6 Joint (SPIFFS) ===");

  // 1. Initialize SPIFFS (Flash Filesystem)
  // SPIFFS chua cac file HTML/JS/CSS duoc upload tu thu muc data/
  if (!SPIFFS.begin(true)) {
    Serial.println("[SPIFFS] Loi: Khong the Initialize SPIFFS!");
    Serial.println("[SPIFFS] Hay chay: pio run -t uploadfs");
    // Tiep tuc de debug (server van hoat dong du khong co file web)
  } else {
    Serial.println("[SPIFFS] Initialize successfully");
    // List files in SPIFFS for debugging
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    while (file) {
      Serial.printf("[SPIFFS] File: %s (%d bytes)\n", file.name(), file.size());
      file = root.openNextFile();
    }
  }

  // 2. Initialize servo (PCA9685) va dua Return to Home
  servo_init();

  // 3. WiFi Connection Station Mode
  Serial.printf("[WiFi] Connecting SSID: %s\n", WIFI_SSID);
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
  Serial.println(">>> Open browser: http://" + WiFi.localIP().toString());

  // 4. Configure WebSocket
  ws_setup(ws);
  server.addHandler(&ws);

  // 5. Configure HTTP routes - serve file tu SPIFFS
  // GET "/" → return /index.html tu SPIFFS
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
    req->send(SPIFFS, "/index.html", "text/html");
  });

  // Serve static files (JS, CSS) tu SPIFFS automatically
  // Vi du: GET "/app.js" → SPIFFS /app.js
  server.serveStatic("/", SPIFFS, "/");

  // 6. Xu ly 404 - File not found
  server.onNotFound([](AsyncWebServerRequest* req) {
    req->send(404, "text/plain", "404: File not found");
  });

  // 7. Start server
  server.begin();
  Serial.println("[Server] HTTP + WebSocket Server has started");
}

// ============================================================
// loop() - Clean up disconnected WebSocket clients
// ============================================================
void loop() {
  static unsigned long lastCleanup = 0;
  if (millis() - lastCleanup > 1000) {
    ws.cleanupClients();
    lastCleanup = millis();
  }
}

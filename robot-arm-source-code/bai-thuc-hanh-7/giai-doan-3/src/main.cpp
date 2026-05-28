// ============================================================
// BAI THUC HANH 7 - GIAI DOAN 3: CHUONG TRINH CHINH
// Mo ta: Web GUI nang cao voi WiFiManager, JSON, Pose Save/Load.
//
// === KIEN TRUC SO SANH 3 BAI ===
// BTH 4-5: PC <--USB Serial--> ESP32
//           Giao thuc: text "M 0 90" | Pham vi: Cuc bo
// BTH 6:   Mobile <--Blynk Cloud--> WiFi --> ESP32
//           Giao thuc: MQTT/Blynk  | Pham vi: Toan cau
// BTH 7:   Browser <--WebSocket--> WiFi LAN --> ESP32
//           Giao thuc: WebSocket JSON | Pham vi: Noi bo LAN
//
// === DIEM MOI GAI DOAN 3 ===
// - WiFiManager: khong hardcode WiFi, cau hinh qua Captive Portal
// - ArduinoJson: parse/serialize JSON thay vi plain-text
// - Preferences (NVS): luu 5 tu the vao bo nho Flash
// - GPIO0 (BOOT): giu > 3s khi boot → xoa WiFi credentials
// ============================================================

#include <Arduino.h>
#include <WiFiManager.h>          // Quan ly WiFi (tzapu/WiFiManager)
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>          // JSON
#include <Preferences.h>          // NVS Flash (luu Pose)

#include "servo_ctrl.h"
#include "ws_handler.h"

// Thoi gian giu GPIO0 de reset WiFi (ms)
#define RESET_BTN_PIN    0        // Nut BOOT = GPIO0
#define RESET_HOLD_MS    3000

// So tu the toi da co the luu
#define MAX_POSES        5

// Doi tuong NVS Preferences de luu/doc tu the
Preferences prefs;

// Doi tuong HTTP server va WebSocket
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// ============================================================
// pose_save() - Luu tu the hien tai vao NVS Flash
// Input: slot (0..MAX_POSES-1)
// NVS key: "pose0" .. "pose4", format: "90,70,90,90,90,90"
// ============================================================
void pose_save(int slot) {
  if (slot < 0 || slot >= MAX_POSES) return;

  String value = "";
  for (int i = 0; i < NUM_JOINTS; i++) {
    value += String(currentAngles[i]);
    if (i < NUM_JOINTS - 1) value += ",";
  }

  prefs.begin("robot", false);
  prefs.putString(("pose" + String(slot)).c_str(), value);
  prefs.end();

  Serial.printf("[Pose] Luu slot %d: %s\n", slot, value.c_str());
}

// ============================================================
// pose_load() - Tai tu the tu NVS Flash va dieu khien servo
// Input: slot (0..MAX_POSES-1)
// ============================================================
bool pose_load(int slot) {
  if (slot < 0 || slot >= MAX_POSES) return false;

  prefs.begin("robot", true);  // read-only
  String value = prefs.getString(("pose" + String(slot)).c_str(), "");
  prefs.end();

  if (value.isEmpty()) {
    Serial.printf("[Pose] Slot %d chua co du lieu\n", slot);
    return false;
  }

  // Parse chuoi "90,70,90,90,90,90" thanh mang int[]
  int angles[6] = {0};
  int count = 0, start = 0;
  while (count < 6) {
    int comma = value.indexOf(',', start);
    String token = (comma == -1) ? value.substring(start)
                                 : value.substring(start, comma);
    angles[count++] = token.toInt();
    if (comma == -1) break;
    start = comma + 1;
  }

  if (count == 6) {
    servo_moveAll(angles);
    Serial.printf("[Pose] Tai slot %d thanh cong\n", slot);
    return true;
  }
  return false;
}

// ============================================================
// checkResetButton() - Kiem tra nut BOOT de reset WiFi
// Giu > 3 giay khi khoi dong → xoa credentials → khoi dong lai
// ============================================================
void checkResetButton() {
  pinMode(RESET_BTN_PIN, INPUT_PULLUP);
  unsigned long pressStart = millis();

  // Neu nut dang duoc giu khi khoi dong
  if (digitalRead(RESET_BTN_PIN) == LOW) {
    Serial.print("[WiFi] Giu nut BOOT: xoa credentials sau 3 giay...");
    while (digitalRead(RESET_BTN_PIN) == LOW) {
      if (millis() - pressStart > RESET_HOLD_MS) {
        WiFiManager wm;
        wm.resetSettings();
        Serial.println(" DA XOA! Khoi dong lai...");
        delay(500);
        ESP.restart();
      }
      Serial.print(".");
      delay(100);
    }
    Serial.println(" Huy (tha nut)");
  }
}

// ============================================================
// setup()
// ============================================================
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n=== BTH 7 - GIAI DOAN 3: WiFiManager + JSON + Pose ===");

  // 1. Kiem tra nut reset truoc khi ket noi WiFi
  checkResetButton();

  // 2. Khoi tao SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("[SPIFFS] Loi! Chay: pio run -t uploadfs");
  } else {
    Serial.println("[SPIFFS] Khoi tao thanh cong");
  }

  // 3. Khoi tao servo
  servo_init();

  // 4. Ket noi WiFi qua WiFiManager
  // - Neu co credentials da luu → ket noi truc tiep
  // - Neu chua co → bat AP "RobotArm-Config" cho cau hinh
  WiFiManager wm;
  wm.setConfigPortalTimeout(180);  // AP config timeout 3 phut
  wm.setAPStaticIPConfig(
    IPAddress(192, 168, 4, 1),
    IPAddress(192, 168, 4, 1),
    IPAddress(255, 255, 255, 0)
  );

  Serial.println("[WiFi] Khoi dong WiFiManager...");
  bool wifiOk = wm.autoConnect("RobotArm-Config", "robotarm123");

  if (!wifiOk) {
    Serial.println("[WiFi] Khong the ket noi! Khoi dong lai sau 10 giay...");
    delay(10000);
    ESP.restart();
  }

  Serial.println("[WiFi] Ket noi thanh cong! IP: " + WiFi.localIP().toString());
  Serial.println(">>> Mo browser: http://" + WiFi.localIP().toString());

  // 5. Cau hinh WebSocket (cung ham ws_setup tu giai doan 2)
  ws_setup(ws);
  server.addHandler(&ws);

  // 6. Them API endpoint cho Pose Save/Load (JSON REST API nho)
  // POST /api/pose/save  body: {"slot":0}
  server.on("/api/pose/save", HTTP_POST,
    [](AsyncWebServerRequest* req) {},
    nullptr,
    [](AsyncWebServerRequest* req, uint8_t* data, size_t len, size_t, size_t) {
      JsonDocument doc;
      if (deserializeJson(doc, data, len) == DeserializationError::Ok) {
        int slot = doc["slot"] | 0;
        pose_save(slot);
        req->send(200, "application/json", "{\"ok\":true}");
      } else {
        req->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      }
    }
  );

  // POST /api/pose/load  body: {"slot":0}
  server.on("/api/pose/load", HTTP_POST,
    [](AsyncWebServerRequest* req) {},
    nullptr,
    [](AsyncWebServerRequest* req, uint8_t* data, size_t len, size_t, size_t) {
      JsonDocument doc;
      if (deserializeJson(doc, data, len) == DeserializationError::Ok) {
        int slot = doc["slot"] | 0;
        bool ok  = pose_load(slot);

        // Tra ve trang thai sau khi load
        JsonDocument resp;
        resp["ok"] = ok;
        if (ok) {
          JsonArray arr = resp["angles"].to<JsonArray>();
          for (int i = 0; i < NUM_JOINTS; i++) arr.add(currentAngles[i]);
        }
        String body;
        serializeJson(resp, body);
        req->send(ok ? 200 : 404, "application/json", body);
      } else {
        req->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      }
    }
  );

  // 7. Phuc vu SPIFFS files
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
    req->send(SPIFFS, "/index.html", "text/html");
  });
  server.serveStatic("/", SPIFFS, "/");
  server.onNotFound([](AsyncWebServerRequest* req) {
    req->send(404, "text/plain", "404 Not Found");
  });

  server.begin();
  Serial.println("[Server] HTTP + WebSocket Server da khoi dong");
}

// ============================================================
// loop()
// ============================================================
void loop() {
  static unsigned long lastCleanup = 0;
  if (millis() - lastCleanup > 1000) {
    ws.cleanupClients();
    lastCleanup = millis();
  }
}

// ============================================================
// BAI THUC HANH 7 - GIAI DOAN 3: WEBSOCKET HANDLER (JSON)
// Mo ta: Nang cap tu plain-text (giai doan 2) sang JSON.
//        So sanh giao thuc:
//        Giai doan 2: "M 0 90"
//        Giai doan 3: {"cmd":"M","joint":0,"angle":90}
//
// JSON cho phep:
//   - Gui nhieu truong du lieu trong 1 tin nhan
//   - Phan biet kieu du lieu ro rang
//   - De mo rong (them truong moi khong anh huong code cu)
// ============================================================

#include "ws_handler.h"
#include "servo_ctrl.h"
#include <ArduinoJson.h>

// ============================================================
// _sendJson() - Gui JSON response ve client
// ============================================================
static void _sendJson(AsyncWebSocketClient* client, JsonDocument& doc) {
  String body;
  serializeJson(doc, body);
  client->text(body);
  Serial.println("[WS] Gui: " + body);
}

// ============================================================
// _parseCommand() - Xu ly lenh JSON tu client
//
// Cac lenh JSON (so sanh voi plain-text giai doan 2):
//   {"cmd":"M","joint":0,"angle":90}   ← "M 0 90"
//   {"cmd":"A","angles":[90,70,90,...]} ← "A 90 70 90 ..."
//   {"cmd":"H"}                         ← "H"
//   {"cmd":"H","joint":0}               ← "H 0"
//   {"cmd":"T"}                         ← "T"
// ============================================================
static void _parseCommand(AsyncWebSocketClient* client, const String& raw) {
  Serial.println("[WS] Nhan: " + raw);

  // Parse JSON
  JsonDocument reqDoc;
  DeserializationError err = deserializeJson(reqDoc, raw);
  if (err) {
    // Neu khong phai JSON hop le → bao loi
    JsonDocument errDoc;
    errDoc["ok"]    = false;
    errDoc["error"] = "JSON khong hop le: " + String(err.c_str());
    _sendJson(client, errDoc);
    return;
  }

  const char* cmd = reqDoc["cmd"] | "";
  JsonDocument respDoc;

  if (strcmp(cmd, "M") == 0) {
    // --- Di chuyen 1 khop ---
    int joint = reqDoc["joint"] | -1;
    int angle = reqDoc["angle"] | 0;
    int actual = servo_move(joint, angle);
    if (actual >= 0) {
      respDoc["ok"]    = true;
      respDoc["joint"] = joint;
      respDoc["angle"] = actual;
    } else {
      respDoc["ok"]    = false;
      respDoc["error"] = "Khop khong hop le";
    }

  } else if (strcmp(cmd, "A") == 0) {
    // --- Di chuyen dong thoi 6 khop ---
    JsonArray arr = reqDoc["angles"];
    if (arr.size() == 6) {
      int angles[6];
      for (int i = 0; i < 6; i++) angles[i] = arr[i];
      servo_moveAll(angles);
      respDoc["ok"] = true;
      JsonArray outArr = respDoc["angles"].to<JsonArray>();
      for (int i = 0; i < NUM_JOINTS; i++) outArr.add(currentAngles[i]);
    } else {
      respDoc["ok"]    = false;
      respDoc["error"] = "Can mang 6 phan tu";
    }

  } else if (strcmp(cmd, "H") == 0) {
    // --- HOME ---
    if (reqDoc.containsKey("joint")) {
      int joint = reqDoc["joint"];
      servo_home(joint);
    } else {
      servo_home(-1);  // Tat ca khop
    }
    respDoc["ok"] = true;
    // Tra kem trang thai sau HOME
    JsonArray arr = respDoc["angles"].to<JsonArray>();
    for (int i = 0; i < NUM_JOINTS; i++) arr.add(currentAngles[i]);

  } else if (strcmp(cmd, "T") == 0) {
    // --- Lay trang thai ---
    respDoc["ok"] = true;
    JsonArray arr = respDoc["angles"].to<JsonArray>();
    for (int i = 0; i < NUM_JOINTS; i++) arr.add(currentAngles[i]);

  } else {
    respDoc["ok"]    = false;
    respDoc["error"] = "Lenh khong xac dinh: " + String(cmd);
  }

  _sendJson(client, respDoc);
}

// ============================================================
// ws_onEvent() - Xu ly su kien WebSocket
// ============================================================
void ws_onEvent(AsyncWebSocket* server, AsyncWebSocketClient* client,
                AwsEventType type, void* arg, uint8_t* data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT: {
      Serial.printf("[WS] Client %u ket noi\n", client->id());
      // Gui trang thai ban dau dang JSON
      JsonDocument doc;
      doc["event"] = "connected";
      JsonArray arr = doc["angles"].to<JsonArray>();
      for (int i = 0; i < NUM_JOINTS; i++) arr.add(currentAngles[i]);
      String body;
      serializeJson(doc, body);
      client->text(body);
      break;
    }

    case WS_EVT_DISCONNECT:
      Serial.printf("[WS] Client %u ngat ket noi\n", client->id());
      break;

    case WS_EVT_DATA: {
      AwsFrameInfo* info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len
          && info->opcode == WS_TEXT) {
        String cmd = String((char*)data, len);
        cmd.trim();
        if (cmd.length() > 0) _parseCommand(client, cmd);
      }
      break;
    }

    default: break;
  }
}

void ws_setup(AsyncWebSocket& ws) {
  ws.onEvent(ws_onEvent);
}

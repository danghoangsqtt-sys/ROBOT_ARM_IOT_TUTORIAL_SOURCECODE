// ============================================================
// LAB 7 - STAGE 3: WEBSOCKET HANDLER (JSON)
// Description: Upgrade from plain-text (STAGE 2) sang JSON.
//        So sanh Protocol:
//        STAGE 2: "M 0 90"
//        STAGE 3: {"cmd":"M","joint":0,"angle":90}
//
// JSON cho phep:
//   - Send nhieu truong du lieu trong 1 tin Receive
//   - Clearly distinguish data types
//   - Easy to expand (them truong moi khong anh huong code cu)
// ============================================================

#include "ws_handler.h"
#include "servo_ctrl.h"
#include <ArduinoJson.h>

// ============================================================
// _sendJson() - Send JSON response ve client
// ============================================================
static void _sendJson(AsyncWebSocketClient* client, JsonDocument& doc) {
  String body;
  serializeJson(doc, body);
  client->text(body);
  Serial.println("[WS] Send: " + body);
}

// ============================================================
// _parseCommand() - Xu ly lenh JSON tu client
//
// Cac lenh JSON (so sanh voi plain-text STAGE 2):
//   {"cmd":"M","joint":0,"angle":90}   ← "M 0 90"
//   {"cmd":"A","angles":[90,70,90,...]} ← "A 90 70 90 ..."
//   {"cmd":"H"}                         ← "H"
//   {"cmd":"H","joint":0}               ← "H 0"
//   {"cmd":"T"}                         ← "T"
// ============================================================
static void _parseCommand(AsyncWebSocketClient* client, const String& raw) {
  Serial.println("[WS] Receive: " + raw);

  // Parse JSON
  JsonDocument reqDoc;
  DeserializationError err = deserializeJson(reqDoc, raw);
  if (err) {
    // If not valid JSON → report error
    JsonDocument errDoc;
    errDoc["ok"]    = false;
    errDoc["error"] = "JSON Invalid: " + String(err.c_str());
    _sendJson(client, errDoc);
    return;
  }

  const char* cmd = reqDoc["cmd"] | "";
  JsonDocument respDoc;

  if (strcmp(cmd, "M") == 0) {
    // --- Di chuyen 1 Joint ---
    int joint = reqDoc["joint"] | -1;
    int angle = reqDoc["angle"] | 0;
    int actual = servo_move(joint, angle);
    if (actual >= 0) {
      respDoc["ok"]    = true;
      respDoc["joint"] = joint;
      respDoc["angle"] = actual;
    } else {
      respDoc["ok"]    = false;
      respDoc["error"] = "Joint Invalid";
    }

  } else if (strcmp(cmd, "A") == 0) {
    // --- Move simultaneously 6 Joint ---
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
      respDoc["error"] = "Can Array of 6 elements";
    }

  } else if (strcmp(cmd, "H") == 0) {
    // --- HOME ---
    if (reqDoc.conLoadnsKey("joint")) {
      int joint = reqDoc["joint"];
      servo_home(joint);
    } else {
      servo_home(-1);  // All Joint
    }
    respDoc["ok"] = true;
    // Return status after HOME
    JsonArray arr = respDoc["angles"].to<JsonArray>();
    for (int i = 0; i < NUM_JOINTS; i++) arr.add(currentAngles[i]);

  } else if (strcmp(cmd, "T") == 0) {
    // --- Lay trang thai ---
    respDoc["ok"] = true;
    JsonArray arr = respDoc["angles"].to<JsonArray>();
    for (int i = 0; i < NUM_JOINTS; i++) arr.add(currentAngles[i]);

  } else {
    respDoc["ok"]    = false;
    respDoc["error"] = "Unknown command: " + String(cmd);
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
      // Send Initial status dang JSON
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

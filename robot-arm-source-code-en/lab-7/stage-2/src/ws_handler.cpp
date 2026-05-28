// ============================================================
// LAB 7 - STAGE 2: WEBSOCKET HANDLER
// Description: Xu ly cac su kien WebSocket va parse lenh plain-text.
//        Protocol: same as LAB 4-5 Serial (M, A, H, T, S, W)
// ============================================================

#include "ws_handler.h"
#include "servo_ctrl.h"

// ============================================================
// _parseCommand() - Ham noi bo: xu ly 1 chuoi lenh
// Protocol (same as LAB 4-5 Serial Protocol):
//   "M <j> <angle>"       → Di chuyen Joint j
//   "A <a0> <a1>...<a5>"  → Move simultaneously 6 Joint
//   "H" / "H <j>"         → Home All hoac 1 Joint
//   "T"                   → Return status hien Load
//   "S <j> <speed>"       → Dat toc do (ghi Receive, not applied)
// ============================================================
static void _parseCommand(AsyncWebSocketClient* client, const String& cmd) {
  Serial.println("[WS] Receive: " + cmd);

  if (cmd.startsWith("M ")) {
    // --- M <joint> <angle>: Di chuyen 1 Joint ---
    int sp1 = cmd.indexOf(' ', 2);
    if (sp1 > 0) {
      int joint = cmd.substring(2, sp1).toInt();
      int angle = cmd.substring(sp1 + 1).toInt();
      int actual = servo_move(joint, angle);
      if (actual >= 0) {
        client->text("OK");
      } else {
        client->text("ERR: Joint Invalid");
      }
    }

  } else if (cmd.startsWith("A ")) {
    // --- A <a0> <a1> <a2> <a3> <a4> <a5>: Move simultaneously ---
    int angles[6] = {0};
    String rest   = cmd.substring(2);
    int count     = 0;
    int start     = 0;
    // Parse 6 gia tri so nguyen ngan cach nhau boi dau cach
    while (count < 6) {
      int sp = rest.indexOf(' ', start);
      String token = (sp == -1) ? rest.substring(start)
                                : rest.substring(start, sp);
      angles[count++] = token.toInt();
      if (sp == -1) break;
      start = sp + 1;
    }
    if (count == 6) {
      servo_moveAll(angles);
      client->text("OK");
    } else {
      client->text("ERR: Need 6 angle values");
    }

  } else if (cmd == "H") {
    // --- H: Home All Joint ---
    servo_home(-1);
    client->text("OK");

  } else if (cmd.startsWith("H ")) {
    // --- H <joint>: Home 1 Joint ---
    int joint = cmd.substring(2).toInt();
    servo_home(joint);
    client->text("OK");

  } else if (cmd == "T") {
    // --- T: Return status hien Load ---
    client->text(servo_getStatusString());

  } else if (cmd.startsWith("S ")) {
    // --- S <joint> <speed>: Record speed (STAGE 2 chua su dung) ---
    // Tinh nang nay se duoc ap dung day du o STAGE 3
    client->text("OK");

  } else {
    client->text("ERR: Unknown command: " + cmd);
  }
}

// ============================================================
// ws_onEvent() - Xu ly cac su kien WebSocket
// ============================================================
void ws_onEvent(AsyncWebSocket* server, AsyncWebSocketClient* client,
                AwsEventType type, void* arg, uint8_t* data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("[WS] Client %u ket noi (IP: %s)\n",
                    client->id(), client->remoteIP().toString().c_str());
      // Send Initial status khi client vua ket noi
      client->text(servo_getStatusString());
      break;

    case WS_EVT_DISCONNECT:
      Serial.printf("[WS] Client %u ngat ket noi\n", client->id());
      break;

    case WS_EVT_DATA: {
      AwsFrameInfo* info = (AwsFrameInfo*)arg;
      // Chi xu ly frame TEXT hoan chinh (khong phan manh)
      if (info->final && info->index == 0 && info->len == len
          && info->opcode == WS_TEXT) {
        String cmd = String((char*)data, len);
        cmd.trim();
        if (cmd.length() > 0) {
          _parseCommand(client, cmd);
        }
      }
      break;
    }

    case WS_EVT_ERROR:
      Serial.printf("[WS] Loi client %u\n", client->id());
      break;

    default:
      break;
  }
}

// ============================================================
// ws_setup() - Dang ky callback vao doi tuong WebSocket
// ============================================================
void ws_setup(AsyncWebSocket& ws) {
  ws.onEvent(ws_onEvent);
}

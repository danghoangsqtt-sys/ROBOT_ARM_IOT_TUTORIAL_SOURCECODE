// ============================================================
// BAI THUC HANH 4 - GIAI DOAN 3: COMMAND PARSER
// Mo ta: Phan tich lenh Serial dieu khien robot arm.
// Cac lenh: M, G, T, H, I, S (speed), A (all), W (wait)
// ============================================================

// === GIAI DOAN 3: Command Parser ===

#include "cmd_parser.h"
#include "servo_ctrl.h"
#include "config.h"

// ------------------------------------------------------------
// Ham cmd_parse(): Phan tich chuoi lenh va thuc thi
// - Cac lenh GD2: M, G, T, H, I
// - Lenh moi GD3: S (set speed), A (move all), W (wait)
// ------------------------------------------------------------
void cmd_parse(String input) {
  char cmd = input.charAt(0);
  String params = "";
  int spaceIdx = input.indexOf(' ');
  if (spaceIdx > 0) {
    params = input.substring(spaceIdx + 1);
    params.trim();
  }

  switch (cmd) {
    // Lenh M <joint> <angle>: Di chuyen khop
    case 'M': case 'm': {
      int sp = params.indexOf(' ');
      if (sp < 0) { Serial.println("ERR:MISSING_PARAM"); return; }
      int joint = params.substring(0, sp).toInt();
      int angle = params.substring(sp + 1).toInt();
      if (joint < 0 || joint >= NUM_JOINTS) {
        Serial.println("ERR:INVALID_JOINT"); return;
      }
      servo_move(joint, angle);
      Serial.println("OK");
      break;
    }

    // Lenh G <joint>: Lay goc hien tai
    case 'G': case 'g': {
      if (params.length() == 0) { Serial.println("ERR:MISSING_PARAM"); return; }
      int joint = params.toInt();
      if (joint < 0 || joint >= NUM_JOINTS) {
        Serial.println("ERR:INVALID_JOINT"); return;
      }
      Serial.print("VAL:"); Serial.print(joint);
      Serial.print(":"); Serial.println(servo_getAngle(joint));
      break;
    }

    // Lenh T: In trang thai tat ca
    case 'T': case 't':
      servo_printStatus();
      break;

    // Lenh H [joint]: Ve home
    case 'H': case 'h': {
      if (params.length() == 0) {
        servo_home(-1);
      } else {
        int joint = params.toInt();
        if (joint < 0 || joint >= NUM_JOINTS) {
          Serial.println("ERR:INVALID_JOINT"); return;
        }
        servo_home(joint);
      }
      Serial.println("OK");
      break;
    }

    // Lenh I: In thong tin cau hinh
    case 'I': case 'i':
      servo_printInfo();
      break;

    // Lenh S <joint> <speed>: Dat toc do cho khop (GD3)
    case 'S': case 's': {
      int sp = params.indexOf(' ');
      if (sp < 0) { Serial.println("ERR:MISSING_PARAM"); return; }
      int joint = params.substring(0, sp).toInt();
      int speed = params.substring(sp + 1).toInt();
      if (joint < 0 || joint >= NUM_JOINTS) {
        Serial.println("ERR:INVALID_JOINT"); return;
      }
      servo_setSpeed(joint, speed);
      Serial.println("OK");
      break;
    }

    // Lenh A <a0> <a1> <a2> <a3> <a4> <a5>: Di chuyen 6 khop (GD3)
    case 'A': case 'a': {
      int angles[6];
      String remaining = params;
      for (int i = 0; i < 6; i++) {
        remaining.trim();
        if (remaining.length() == 0 && i < 6) {
          Serial.println("ERR:NEED_6_ANGLES"); return;
        }
        int sp = remaining.indexOf(' ');
        if (sp < 0 && i < 5) {
          Serial.println("ERR:NEED_6_ANGLES"); return;
        }
        if (sp < 0) {
          angles[i] = remaining.toInt();
        } else {
          angles[i] = remaining.substring(0, sp).toInt();
          remaining = remaining.substring(sp + 1);
        }
      }
      servo_moveAll(angles);
      Serial.println("OK");
      break;
    }

    // Lenh W: Cho hoan thanh (GD3)
    case 'W': case 'w':
      servo_wait();
      break;

    default:
      Serial.println("ERR:INVALID_CMD");
      break;
  }
}

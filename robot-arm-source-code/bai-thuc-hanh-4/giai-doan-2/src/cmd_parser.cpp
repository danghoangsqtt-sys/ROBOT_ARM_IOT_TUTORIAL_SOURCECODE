// ============================================================
// BAI THUC HANH 4 - GIAI DOAN 2: COMMAND PARSER
// Mo ta: Phan tich lenh Serial dieu khien robot arm.
// Cac lenh: M (move), G (get), T (status), H (home), I (info)
// ============================================================

// === GIAI DOAN 2: Command Parser ===

#include "cmd_parser.h"
#include "servo_ctrl.h"
#include "config.h"

// ------------------------------------------------------------
// Ham cmd_parse(): Phan tich chuoi lenh va thuc thi
// - Ky tu dau tien la ma lenh (M, G, T, H, I)
// - Cac tham so theo sau cach boi dau cach
// - Tra ve ket qua hoac ma loi qua Serial
// ------------------------------------------------------------
void cmd_parse(String input) {
  char cmd = input.charAt(0);
  String params = "";

  // Tach phan tham so sau ky tu lenh
  int spaceIdx = input.indexOf(' ');
  if (spaceIdx > 0) {
    params = input.substring(spaceIdx + 1);
    params.trim();
  }

  switch (cmd) {
    // Lenh M <joint> <angle>: Di chuyen khop den goc chi dinh
    case 'M':
    case 'm': {
      int sp = params.indexOf(' ');
      if (sp < 0) { Serial.println("ERR:MISSING_PARAM"); return; }
      int joint = params.substring(0, sp).toInt();
      int angle = params.substring(sp + 1).toInt();
      if (joint < 0 || joint >= NUM_JOINTS) {
        Serial.println("ERR:INVALID_JOINT");
        return;
      }
      servo_move(joint, angle);
      Serial.println("OK");
      break;
    }

    // Lenh G <joint>: Lay goc hien tai cua khop
    case 'G':
    case 'g': {
      if (params.length() == 0) { Serial.println("ERR:MISSING_PARAM"); return; }
      int joint = params.toInt();
      if (joint < 0 || joint >= NUM_JOINTS) {
        Serial.println("ERR:INVALID_JOINT");
        return;
      }
      Serial.print("VAL:");
      Serial.print(joint);
      Serial.print(":");
      Serial.println(servo_getAngle(joint));
      break;
    }

    // Lenh T: In trang thai tat ca cac khop
    case 'T':
    case 't':
      servo_printStatus();
      break;

    // Lenh H [joint]: Dua ve home (khong tham so = tat ca)
    case 'H':
    case 'h': {
      if (params.length() == 0) {
        servo_home(-1);
      } else {
        int joint = params.toInt();
        if (joint < 0 || joint >= NUM_JOINTS) {
          Serial.println("ERR:INVALID_JOINT");
          return;
        }
        servo_home(joint);
      }
      Serial.println("OK");
      break;
    }

    // Lenh I: In bang thong tin cau hinh
    case 'I':
    case 'i':
      servo_printInfo();
      break;

    // Lenh khong hop le
    default:
      Serial.println("ERR:INVALID_CMD");
      break;
  }
}

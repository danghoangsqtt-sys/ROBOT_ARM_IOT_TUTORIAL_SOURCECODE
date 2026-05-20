# =============================================
# BAI THUC HANH 5 - GIAI DOAN 1
# File: robot_basic.py
# Mo ta: Ket noi Serial co ban voi ESP32,
#        gui lenh dieu khien Robot Arm
# =============================================
# === GIAI DOAN 1: KET NOI SERIAL CO BAN ===

import serial
import time

# --- Cau hinh cong Serial ---
PORT = 'COM9'   # <-- THAY BANG COM PORT THUC TE CUA BAN
BAUD = 115200   # Toc do baud phai khop voi firmware ESP32

# Mo cong Serial voi timeout 2 giay
# timeout=2 nghia la ham readline() se cho toi da 2 giay de nhan du lieu
robot = serial.Serial(PORT, BAUD, timeout=2)

# Cho 2 giay de ESP32 hoan tat reset sau khi mo cong Serial
# ESP32 tu dong reset khi co ket noi Serial moi
time.sleep(2)
print(f"Da ket noi voi {PORT} @ {BAUD} baud")


def send(cmd: str) -> str:
    """Gui lenh xuong Robot va nhan phan hoi.
    Moi lenh phai ket thuc bang ky tu xuong dong '\\n'
    de firmware nhan biet het lenh."""
    robot.write((cmd + '\n').encode())
    response = robot.readline().decode().strip()
    print(f"  >> {cmd:25s} | << {response}")
    return response


# --- Trinh dien cac lenh co ban ---
print("\n--- BAT DAU DEMO ---")

send('H')           # Dua tat ca cac khop ve vi tri Home
send('W')           # Cho cho den khi chuyen dong hoan tat

send('M 0 45')      # Di chuyen khop J0 (Base) den goc 45 do
send('W')           # Cho hoan tat

send('T')           # Doc trang thai tat ca cac khop

send('M 0 135')     # Di chuyen khop J0 den goc 135 do
send('W')           # Cho hoan tat

send('H')           # Dua ve Home lai
send('W')           # Cho hoan tat

# --- Dong cong Serial ---
robot.close()
print("\n--- KET THUC DEMO ---")
print("Done.")

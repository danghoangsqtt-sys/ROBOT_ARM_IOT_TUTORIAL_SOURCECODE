# =============================================
# BAI THUC HANH 5 - GIAI DOAN 2
# File: robot_sequence.py
# Mo ta: Chuoi hanh dong tu dong pick-and-place
# =============================================
# === GIAI DOAN 2: CHUOI HANH DONG TU DONG ===

import serial
import time

# --- Cau hinh cong Serial ---
PORT = 'COM9'   # <-- THAY BANG COM PORT THUC TE CUA BAN
BAUD = 115200

# Mo cong Serial va cho ESP32 reset
robot = serial.Serial(PORT, BAUD, timeout=2)
time.sleep(2)
print(f"Da ket noi voi {PORT} @ {BAUD} baud")


def send(cmd: str) -> str:
    """Gui lenh va nhan phan hoi tu Robot."""
    robot.write((cmd + '\n').encode())
    response = robot.readline().decode().strip()
    print(f"  >> {cmd:30s} | << {response}")
    return response


# --- Dinh nghia chuoi hanh dong pick-and-place ---
# Moi phan tu la mot tuple (lenh, mo_ta)
# Tuple giup gom lenh va mo ta thanh mot cap, de doc va bao tri
sequence = [
    ('H',                        'Ve Home'),
    ('S 0 3',                    'Giam toc do J0'),
    ('M 5 60',                   'Mo kep'),
    ('A 90 120 130 90 90 60',    'Tiep can vi tri A'),
    ('A 90 140 140 90 90 60',    'Ha xuong'),
    ('M 5 110',                  'Gap vat'),
    ('A 90 100 90 90 90 110',    'Nang len'),
    ('M 0 45',                   'Xoay den vi tri B'),
    ('A 45 130 130 90 90 110',   'Ha xuong'),
    ('M 5 60',                   'Tha vat'),
    ('H',                        'Ve Home'),
]

# --- Thuc thi chuoi hanh dong ---
print("\n=== BAT DAU PICK-AND-PLACE ===\n")

for step_index, (cmd, description) in enumerate(sequence, start=1):
    # In buoc hien tai voi mo ta bang tieng Viet
    print(f"[Buoc {step_index:2d}/{len(sequence)}] {description}")

    # Gui lenh dieu khien
    send(cmd)

    # Gui lenh W (Wait) sau moi buoc de dam bao Robot
    # hoan tat chuyen dong truoc khi thuc hien buoc tiep theo.
    # Neu khong co W, cac lenh se chong cheo len nhau.
    send('W')

    print()  # Dong trong cho de doc

# --- Ket thuc ---
robot.close()
print("=== HOAN TAT PICK-AND-PLACE ===")
print("Done.")

# =============================================
# LAB 5 - STAGE 2
# File: robot_sequence.py
# Description: Chuoi hanh dong tu dong pick-and-place
# =============================================
# === STAGE 2: CHUOI HANH DONG TU DONG ===

import serial
import time

# --- Cau hinh cong Serial ---
PORT = 'COM9'   # <-- THAY BANG COM PORT THUC TE CUA BAN
BAUD = 115200

# Open Serial port va Wait for ESP32 reset
robot = serial.Serial(PORT, BAUD, timeout=2)
time.sleep(2)
print(f"Da ket noi voi {PORT} @ {BAUD} baud")


def send(cmd: str) -> str:
    """Send command va nhan phan hoi tu Robot."""
    robot.write((cmd + '\n').encode())
    response = robot.readline().decode().strip()
    print(f"  >> {cmd:30s} | << {response}")
    return response


# --- Dinh nghia Pick-and-place action sequence ---
# Moi phan tu la mot tuple (lenh, mo_ta)
# Tuple giup gom lenh va mo ta thanh mot cap, de doc va bao tri
sequence = [
    ('H',                        'Return to Home'),
    ('S 0 3',                    'Reduce speed J0'),
    ('M 5 60',                   'Open gripper'),
    ('A 90 120 130 90 90 60',    'Approach position A'),
    ('A 90 140 140 90 90 60',    'Lower'),
    ('M 5 110',                  'Grip object'),
    ('A 90 100 90 90 90 110',    'Lift up'),
    ('M 0 45',                   'Rotate to position B'),
    ('A 45 130 130 90 90 110',   'Lower'),
    ('M 5 60',                   'Release object'),
    ('H',                        'Return to Home'),
]

# --- Thuc thi chuoi hanh dong ---
print("\n=== BAT DAU PICK-AND-PLACE ===\n")

for step_index, (cmd, description) in enumerate(sequence, start=1):
    # In buoc hien tai voi mo ta bang tieng Viet
    print(f"[Buoc {step_index:2d}/{len(sequence)}] {description}")

    # Send command dieu khien
    send(cmd)

    # Send command W (Wait) sau moi buoc de dam bao Robot
    # hoan tat chuyen dong truoc khi thuc hien buoc tiep theo.
    # Neu khong co W, cac lenh se chong cheo len nhau.
    send('W')

    print()  # Dong trong cho de doc

# --- Ket thuc ---
robot.close()
print("=== HOAN TAT PICK-AND-PLACE ===")
print("Done.")

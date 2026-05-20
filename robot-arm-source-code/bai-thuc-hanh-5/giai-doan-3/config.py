# =============================================
# BAI THUC HANH 5 - GIAI DOAN 3
# File: config.py
# Mo ta: Tham so cau hinh cho GUI Robot Arm
# =============================================
# === GIAI DOAN 3: GUI TKINTER HOAN CHINH ===

# --- Tham so giao tiep Serial ---
BAUD_RATE = 115200      # Toc do baud khop voi firmware ESP32
TIMEOUT = 2             # Thoi gian cho phan hoi toi da (giay)
INIT_DELAY = 2          # Thoi gian cho ESP32 reset (giay)

# --- Cau hinh cac khop (Joint) ---
# Moi khop duoc mo ta bang dict voi cac truong:
#   name    : Ten hien thi tren GUI
#   channel : So thu tu khop (0-5)
#   min     : Goc nho nhat (do)
#   max     : Goc lon nhat (do)
#   home    : Goc vi tri Home
JOINTS = [
    {"name": "J0 - Base",        "channel": 0, "min": 0,  "max": 180, "home": 90},
    {"name": "J1 - Shoulder",    "channel": 1, "min": 70, "max": 150, "home": 70},
    {"name": "J2 - Elbow",       "channel": 2, "min": 0,  "max": 150, "home": 90},
    {"name": "J3 - Wrist Pitch", "channel": 3, "min": 0,  "max": 180, "home": 90},
    {"name": "J4 - Wrist Roll",  "channel": 4, "min": 0,  "max": 180, "home": 90},
    {"name": "J5 - Gripper",     "channel": 5, "min": 60, "max": 120, "home": 90},
]

# --- Mau sac giao dien GUI (theme toi) ---
BG_COLOR = "#2b2b2b"        # Mau nen chinh
SLIDER_COLOR = "#4a9eff"    # Mau thanh truot slider
BTN_COLOR = "#3c3c3c"       # Mau nen nut bam
LOG_COLOR = "#1e1e1e"       # Mau nen khung log

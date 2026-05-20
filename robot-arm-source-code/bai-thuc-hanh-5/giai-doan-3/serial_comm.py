# =============================================
# BAI THUC HANH 5 - GIAI DOAN 3
# File: serial_comm.py
# Mo ta: Module giao tiep Serial voi Robot Arm
# =============================================
# === GIAI DOAN 3: GUI TKINTER HOAN CHINH ===

import serial
import time
from config import BAUD_RATE, TIMEOUT, INIT_DELAY


class RobotSerial:
    """Lop quan ly giao tiep Serial voi Robot Arm ESP32."""

    def __init__(self):
        """Khoi tao trang thai ban dau: chua ket noi."""
        self.ser = None
        self.connected = False

    def connect(self, port: str) -> bool:
        """Mo cong Serial va cho ESP32 reset.
        Tra ve True neu ket noi thanh cong, False neu that bai."""
        try:
            self.ser = serial.Serial(port, BAUD_RATE, timeout=TIMEOUT)
            # Cho ESP32 hoan tat qua trinh reset
            time.sleep(INIT_DELAY)

            # Doc cac dong khoi tao ma ESP32 gui ra khi boot
            while self.ser.in_waiting:
                init_line = self.ser.readline().decode().strip()
                if init_line:
                    print(f"[INIT] {init_line}")

            self.connected = True
            print(f"[OK] Da ket noi {port} @ {BAUD_RATE} baud")
            return True

        except serial.SerialException as e:
            print(f"[LOI] Khong the ket noi {port}: {e}")
            self.connected = False
            return False

    def disconnect(self):
        """Dong cong Serial va cap nhat trang thai."""
        if self.ser and self.ser.is_open:
            self.ser.close()
            print("[OK] Da ngat ket noi Serial")
        self.connected = False
        self.ser = None

    def send(self, cmd: str) -> str:
        """Gui lenh den Robot va tra ve phan hoi.
        Tra ve chuoi rong neu chua ket noi, 'ERR:TIMEOUT' neu het thoi gian cho."""
        if not self.connected or not self.ser:
            return ""
        try:
            self.ser.write((cmd + '\n').encode())
            response = self.ser.readline().decode().strip()
            print(f"  >> {cmd:25s} | << {response}")
            return response
        except serial.SerialException as e:
            print(f"[LOI] Gui lenh that bai: {e}")
            return "ERR:TIMEOUT"
        except UnicodeDecodeError:
            print("[LOI] Khong the giai ma phan hoi")
            return "ERR:TIMEOUT"

    def get_status(self) -> list:
        """Doc trang thai tat ca cac khop bang lenh T.
        Tra ve danh sach goc [a0, a1, a2, a3, a4, a5].
        Tra ve danh sach rong neu co loi."""
        response = self.send('T')
        # Phan hoi co dang: STA:a0,a1,a2,a3,a4,a5
        if response.startswith('STA:'):
            try:
                values = response[4:]  # Bo phan 'STA:'
                angles = [int(v) for v in values.split(',')]
                return angles
            except ValueError:
                print("[LOI] Khong the phan tich du lieu trang thai")
                return []
        return []

    def is_connected(self) -> bool:
        """Kiem tra trang thai ket noi hien tai."""
        return self.connected

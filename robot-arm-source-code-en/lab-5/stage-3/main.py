# =============================================
# LAB 5 - STAGE 3
# File: main.py
# Description: GUI Tkinter dieu khien Robot Arm 6DOF
# =============================================
# === STAGE 3: GUI TKINTER HOAN CHINH ===

import tkinter as tk
from tkinter import ttk
import serial.tools.list_ports
from config import JOINTS, BG_COLOR, SLIDER_COLOR, BTN_COLOR, LOG_COLOR
from serial_comm import RobotSerial

# ------------------------------------------
# Class RobotGUI: Giao dien dieu khien Robot
# - Panel ket noi: chon COM port, Connect
# - 6 thanh truot: dieu khien tung khop
# - Nut: HOME, STATUS, EMERGENCY STOP
# - Log: hien thi lenh va phan hoi
# ------------------------------------------
class RobotGUI:
    def __init__(self, root: tk.Tk):
        self.root = root
        root.title("Robot Arm 6DOF Controller")
        root.geometry("700x500")
        root.configure(bg=BG_COLOR)
        self.robot = RobotSerial()
        self.sliders = []
        self.labels = []
        self.debounce_id = None
        self._build_ui()

    def _build_ui(self):
        # --- Panel ket noi (tren cung) ---
        fr_conn = tk.Frame(self.root, bg=BG_COLOR)
        fr_conn.pack(fill=tk.X, padx=10, pady=5)
        tk.Label(fr_conn, text="COM Port:", bg=BG_COLOR, fg="white").pack(side=tk.LEFT)
        self.port_cb = ttk.Combobox(fr_conn, width=12)
        ports = [p.device for p in serial.tools.list_ports.comports()]
        self.port_cb['values'] = ports if ports else ['COM9']
        if ports:
            self.port_cb.current(0)
        self.port_cb.pack(side=tk.LEFT, padx=5)
        self.conn_btn = tk.Button(fr_conn, text="Connect", bg=BTN_COLOR,
                                  fg="white", command=self._toggle_connect)
        self.conn_btn.pack(side=tk.LEFT, padx=5)
        # --- Panel dieu khien: 6 thanh truot (giua) ---
        fr_ctrl = tk.Frame(self.root, bg=BG_COLOR)
        fr_ctrl.pack(fill=tk.BOTH, expand=True, padx=10)
        for j in JOINTS:
            row = tk.Frame(fr_ctrl, bg=BG_COLOR)
            row.pack(fill=tk.X, pady=1)
            lbl = tk.Label(row, text=f"{j['name']}: {j['home']}",
                           width=20, anchor=tk.W, bg=BG_COLOR, fg=SLIDER_COLOR)
            lbl.pack(side=tk.LEFT)
            ch = j['channel']
            sl = tk.Scale(row, from_=j['min'], to=j['max'], orient=tk.HORIZONTAL,
                          bg=BG_COLOR, fg="white", troughcolor=SLIDER_COLOR,
                          highlightthickness=0, length=380,
                          command=lambda v, jid=ch: self._on_slider(jid, v))
            sl.set(j['home'])
            sl.pack(side=tk.LEFT, fill=tk.X, expand=True)
            self.sliders.append(sl)
            self.labels.append(lbl)
        # --- Panel nut chuc nang ---
        fr_btn = tk.Frame(self.root, bg=BG_COLOR)
        fr_btn.pack(fill=tk.X, padx=10, pady=5)
        tk.Button(fr_btn, text="HOME", bg="#2e7d32", fg="white",
                  width=12, command=self._home).pack(side=tk.LEFT, padx=3)
        tk.Button(fr_btn, text="STATUS", bg=BTN_COLOR, fg="white",
                  width=12, command=self._status).pack(side=tk.LEFT, padx=3)
        tk.Button(fr_btn, text="EMERGENCY STOP", bg="#ff4444", fg="white",
                  width=16, command=self._emergency).pack(side=tk.RIGHT, padx=3)
        # --- Panel log (duoi cung) ---
        fr_log = tk.Frame(self.root, bg=BG_COLOR)
        fr_log.pack(fill=tk.X, padx=10, pady=(0, 10))
        self.log_txt = tk.Text(fr_log, height=6, bg=LOG_COLOR, fg="#00ff88",
                               state=tk.DISABLED, font=("Consolas", 9))
        self.log_txt.pack(fill=tk.X)

    def _log(self, msg: str):
        """Ghi log va tu dong cuon xuong dong moi nhat."""
        self.log_txt.configure(state=tk.NORMAL)
        self.log_txt.insert(tk.END, msg + "\n")
        self.log_txt.see(tk.END)
        self.log_txt.configure(state=tk.DISABLED)

    def _toggle_connect(self):
        if self.robot.is_connected():
            self.robot.disconnect()
            self.conn_btn.configure(text="Connect")
            self._log("Da ngat ket noi")
        else:
            port = self.port_cb.get()
            if self.robot.connect(port):
                self.conn_btn.configure(text="Disconnect")
                self._log(f"Da ket noi {port}")
                self._sync_sliders()
            else:
                self._log(f"LOI: Khong the ket noi {port}")

    def _on_slider(self, joint_id: int, value):
        """Xu ly su kien keo thanh truot voi debounce 100ms."""
        angle = int(float(value))
        j = JOINTS[joint_id]
        self.labels[joint_id].configure(text=f"{j['name']}: {angle}")
        if self.debounce_id:
            self.root.after_cancel(self.debounce_id)
        self.debounce_id = self.root.after(
            100, lambda: self._log(f"M {joint_id} {angle} -> "
                                   f"{self.robot.send(f'M {joint_id} {angle}')}"))

    def _home(self):
        self.robot.send('H')
        self.robot.send('W')
        for j in JOINTS:
            self.sliders[j['channel']].set(j['home'])
            self.labels[j['channel']].configure(text=f"{j['name']}: {j['home']}")
        self._log("HOME: Hoan tat")

    def _status(self):
        angles = self.robot.get_status()
        self._log(f"STATUS: {angles}" if angles else "STATUS: Khong co du lieu")

    def _emergency(self):
        self.robot.send('H')
        self._log("!!! EMERGENCY STOP - Da Send command Home khan cap !!!")

    def _sync_sliders(self):
        """Dong bo slider voi trang thai thuc te cua Robot khi ket noi."""
        angles = self.robot.get_status()
        if angles and len(angles) >= len(JOINTS):
            for i, j in enumerate(JOINTS):
                self.sliders[i].set(angles[i])
                self.labels[i].configure(text=f"{j['name']}: {angles[i]}")

if __name__ == "__main__":
    root = tk.Tk()
    app = RobotGUI(root)
    root.mainloop()

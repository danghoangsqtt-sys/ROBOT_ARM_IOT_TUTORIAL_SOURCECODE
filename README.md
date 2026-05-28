# 🤖 Practical Course: 6DOF Robot Arm with ESP32 & IoT

![PlatformIO](https://img.shields.io/badge/PlatformIO-orange?style=for-the-badge&logo=PlatformIO)
![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Python](https://img.shields.io/badge/Python-3776AB?style=for-the-badge&logo=python&logoColor=white)
![Blynk](https://img.shields.io/badge/Blynk_IoT-17b6a4?style=for-the-badge&logo=blynk&logoColor=white)
![WebSocket](https://img.shields.io/badge/WebSocket-ESPAsyncWebServer-blue?style=for-the-badge&logo=javascript&logoColor=white)

Welcome to the sample source code repository for the **6DOF Robot Arm Control Practical Course**. This project provides complete C/C++ source code (for the ESP32 microcontroller) and Python (for the computer GUI) from basic to advanced levels, combined with the Internet of Things (Blynk) ecosystem.

---

## 📸 1. Hardware Overview

Below are the hardware components used in the course:

<div align="center">
  <img src="docs/images/cac-thanh-phan-linh-kien.png" alt="Hardware Component Layout" width="600"/>
  <p><i>Overall layout of the Robot Arm components</i></p>
</div>

### Detailed List:

| Component | Main Function |
| :--- | :--- |
| **ESP32 DevKit V1** | Central microcontroller, logic processing, Serial reading, and WiFi/Blynk connection. |
| **PCA9685 Module** | I2C communication, outputs 16-channel PWM to control multiple Servos simultaneously. |
| **6DOF Robot Arm** | Aluminum alloy actuator mechanism, includes 6 MG996R/MG995 Servo motors. |
| **6V - 15A Power Adapter** | Provides sufficient current for all 6 MG996R/MG995 Servos to operate simultaneously under full load without voltage drops. (**Do not use a 3A supply** — peak stall current per servo can reach 2.5A, totalling up to ~15A for 6 servos.) |

---

## ⚙️ 2. Wiring Diagram

The system uses the **I2C** protocol to connect the ESP32 and PCA9685:

<div align="center">
  <img src="docs/images/so-do-dau-noi-phan-cung.png" alt="Hardware Wiring Diagram" width="700"/>
  <p><i>Detailed diagram of signal pins and power connections</i></p>
</div>

*   **ESP32 `GPIO 21`** $\rightarrow$ PCA9685 `SDA`
*   **ESP32 `GPIO 22`** $\rightarrow$ PCA9685 `SCL`
*   **ESP32 `GND`** $\rightarrow$ PCA9685 `GND`
*   **ESP32 `3V3`** $\rightarrow$ PCA9685 `VCC` (Logic power supply)
*   **External Power 6V/15A** $\rightarrow$ Green Terminal of PCA9685 (Dedicated power for Servos, note **V+ / GND**). ⚠️ *Use at least 15A — each servo can draw up to 2.5A at stall.*

---

## 📂 3. Project Structure

The project contains **two language versions** of the source code, each with a dedicated VS Code workspace:

| Workspace File | Language | Description |
| :--- | :--- | :--- |
| **`RobotArmTutorial-EN.code-workspace`** | 🇬🇧 English | English-translated source code & comments |
| **`RobotArmTutorial-VI.code-workspace`** | 🇻🇳 Vietnamese | Original Vietnamese source code & comments |
| `RobotArmTutorial.code-workspace` | Both | Combined view (both versions) |

Each version is organized as a **Multi-root Workspace** in VS Code, divided into 3 major Labs, each with 3 progressive stages:

```text
ROBOT_ARM_IOT_TUTORIAL_SOURCECODE/
├── RobotArmTutorial-EN.code-workspace   ← [OPEN THIS for English version]
├── RobotArmTutorial-VI.code-workspace   ← [OPEN THIS for Vietnamese version]
├── robot-arm-source-code-en/            ← English Version
│   ├── lab-4/                           ← C++ Programming (PlatformIO)
│   │   ├── stage-1/                     (I2C Scanner)
│   │   ├── stage-2/                     (Basic Servo Control, Serial CLI)
│   │   └── stage-3/                     (Smooth move, simultaneous movement)
│   ├── lab-5/                           ← Python GUI Programming
│   │   ├── stage-1/                     (Basic Serial Communication)
│   │   ├── stage-2/                     (Auto Pick-and-Place)
│   │   └── stage-3/                     (Complete Tkinter GUI)
│   ├── lab-6/                           ← IoT Programming (Blynk)
│   │   ├── stage-1/                     (WiFi connection, 1 joint control)
│   │   ├── stage-2/                     (6 joints control, state synchronization)
│   │   └── stage-3/                     (Optimization, Mobile App & Auto Reconnect)
│   └── lab-7/                           ← Web GUI (ESPAsyncWebServer) [Phase 2]
└── robot-arm-source-code/               ← Vietnamese Version (Bản Tiếng Việt)
    ├── bai-thuc-hanh-4/                 ← Lập trình C++ (PlatformIO)
    │   ├── giai-doan-1/                 (I2C Scanner)
    │   ├── giai-doan-2/                 (Điều khiển Servo, Serial CLI)
    │   └── giai-doan-3/                 (Chuyển động mượt, đồng thời)
    ├── bai-thuc-hanh-5/                 ← Lập trình Python GUI
    │   ├── giai-doan-1/                 (Giao tiếp Serial cơ bản)
    │   ├── giai-doan-2/                 (Pick-and-Place tự động)
    │   └── giai-doan-3/                 (GUI Tkinter hoàn chỉnh)
    ├── bai-thuc-hanh-6/                 ← Lập trình IoT (Blynk)
    │   ├── giai-doan-1/                 (Kết nối WiFi, điều khiển 1 khớp)
    │   └── giai-doan-3/                 (Hoàn thiện, Mobile App & Tự kết nối lại)
    ├── bai-thuc-hanh-7/                 ← Web GUI (ESPAsyncWebServer) [MỚI]
    │   ├── giai-doan-1/                 (WiFi STA + WebSocket + 1 servo, HTML PROGMEM)
    │   ├── giai-doan-2/                 (SPIFFS + Web GUI 6 khớp hoàn chỉnh)
    │   └── giai-doan-3/                 (WiFiManager + JSON Protocol + Pose Save/Load)
    └── bai-thuc-hanh-8/                 ← Digital Twin & Web Serial [MỚI CHỐT]
        ├── giai-doan-1/                 (Web Serial API - HTML/JS gọi cổng COM USB)
        └── giai-doan-2/                 (3D Digital Twin + Sequence Recorder + REST API)
```

---

## 🚀 4. Usage Instructions

### Step 1 — Choose your language version

| Version | Workspace to open |
| :--- | :--- |
| 🇬🇧 **English** | `RobotArmTutorial-EN.code-workspace` |
| 🇻🇳 **Vietnamese** | `RobotArmTutorial-VI.code-workspace` |

### For C/C++ Developers (PlatformIO)
1. Install **Visual Studio Code** and the **PlatformIO IDE** Extension.
2. Open VS Code $\rightarrow$ `File` $\rightarrow$ `Open Workspace from File...` $\rightarrow$ Select the workspace file for your preferred language (see table above).
3. In the Status Bar at the bottom, select the environment of the Stage you want to upload.
4. Open the corresponding `platformio.ini` file and change `upload_port = COM9` to your actual COM port.
5. Click the **Upload (right arrow)** button to flash the code to the ESP32.

### For Python GUI
1. Install Python 3.8+.
2. Open the Terminal at the `lab-5/stage-3` directory (English) or `bai-thuc-hanh-5/giai-doan-3` (Vietnamese).
3. Install the required libraries: `pip install -r requirements.txt` (mainly `pyserial`).
4. Run the GUI: `python main.py`


---

## 📜 5. Serial Protocol

The firmware supports plain text-based commands via the Serial port at a baud rate of **115200**, ending with a newline character `\n`.

| Command | Meaning | Syntax | Response |
| :--- | :--- | :--- | :--- |
| **M** | Move 1 joint | `M <joint_id> <angle>` (e.g., `M 0 90`) | `OK` |
| **G** | Get joint angle | `G <joint_id>` | `VAL:<id>:<angle>` |
| **T** | Get total status | `T` | `STA:a0,a1,a2,a3,a4,a5` |
| **H** | Go to Home | `H` (All) or `H <joint_id>` | `OK` |
| **S** | Set speed | `S <joint_id> <speed>` (Speed 1-10) | `OK` |
| **A** | Move simultaneously | `A <a0> <a1> <a2> <a3> <a4> <a5>` | `OK` |
| **W** | Wait for movement | `W` | `DONE` |

---

## ⚡ 6. PWM & Servo Control — Technical Reference

This section explains how PWM signals are used to control the servo motors via the **PCA9685** module.

### 6.1 PWM Signal Basics

A standard RC servo motor is controlled by a **Pulse Width Modulated (PWM)** signal:

| Parameter | Value | Notes |
| :--- | :--- | :--- |
| **PWM Frequency** | **50 Hz** | Period = 20 ms (standard for all RC servos) |
| **Min Pulse Width** | **0.5 ms** | Corresponds to **0°** |
| **Max Pulse Width** | **2.5 ms** | Corresponds to **180°** |
| **Neutral Pulse** | **1.5 ms** | Corresponds to **90°** (home position) |

```
   20 ms period (50 Hz)
  ┌──────────────────────────────────────────┐
  │                                          │
──┘◄0.5ms►┐                         High = 0°  (SERVOMIN)
  │       │
──┘◄─────1.5ms─────►┐               High = 90° (midpoint)
  │                  │
──┘◄──────────2.5ms──────────►┐     High = 180°(SERVOMAX)
```

### 6.2 PCA9685 Tick Resolution

The PCA9685 operates at **12-bit resolution** (4096 steps per period).
With a 50 Hz frequency (20 ms period), each tick = 20 ms ÷ 4096 ≈ **4.88 µs**.

| Angle | Pulse Width | PCA9685 Ticks (SERVOMIN/MAX) |
| :---: | :---: | :---: |
| **0°** | 0.5 ms | **102** ticks |
| **90°** | 1.5 ms | **307** ticks |
| **180°** | 2.5 ms | **512** ticks |

> **How 102 and 512 are derived:**
> - `SERVOMIN = 0.5 ms / (20 ms / 4096) = 0.5 / 0.00488 ≈ 102`
> - `SERVOMAX = 2.5 ms / (20 ms / 4096) = 2.5 / 0.00488 ≈ 512`

### 6.3 Angle → Pulse Width Formula

To move a servo to any angle between 0° and 180°, the firmware maps the angle to a PCA9685 tick count:

```cpp
// Linear mapping: angle [0°, 180°] → tick count [SERVOMIN, SERVOMAX]
int pulse = map(angle, 0, 180, SERVOMIN, SERVOMAX);
//  where:  SERVOMIN = 102   (0.5 ms pulse → 0°)
//          SERVOMAX = 512   (2.5 ms pulse → 180°)

pca9685.setPWM(channel, 0, pulse);  // Send to PCA9685
```

**Example calculations:**

| Target Angle | Formula | Pulse (ticks) | Pulse Width |
| :---: | :--- | :---: | :---: |
| 0° | `map(0,   0, 180, 102, 512)` | 102 | 0.50 ms |
| 45° | `map(45,  0, 180, 102, 512)` | 204 | 0.99 ms |
| 90° | `map(90,  0, 180, 102, 512)` | 307 | 1.50 ms |
| 135° | `map(135, 0, 180, 102, 512)` | 409 | 2.00 ms |
| 180° | `map(180, 0, 180, 102, 512)` | 512 | 2.50 ms |

### 6.4 Power Supply Requirement

> [!WARNING]
> **Never power all 6 servos from a 3A supply.**
> Each MG996R/MG995 servo can draw up to **~2.5A at stall**. With 6 servos, peak current demand can reach **~15A**. Under-powered servos will cause erratic movement, voltage drops, and ESP32 resets.

| Config | Current Draw | Recommendation |
| :--- | :--- | :--- |
| 1 servo (idle) | ~150 mA | — |
| 1 servo (moving) | ~500–900 mA | — |
| 1 servo (stall) | ~2.5 A | — |
| **6 servos (full load)** | **up to ~15 A** | **Use 6V / 15A supply** |

---

## 🌐 7. Web GUI Protocol (BTH 7 / Lab 7)

Bai Thuc Hanh 7 introduces a **third control architecture** — the browser connects directly to the ESP32 via WebSocket over WiFi LAN:

### Architecture Comparison

| Architecture | Lab | Protocol | Dependency | Scope |
| :--- | :---: | :--- | :--- | :--- |
| Serial USB | BTH 4-5 | Plain-text 115200 baud | PC + USB cable | Local only |
| Blynk Cloud | BTH 6 | MQTT / Blynk | Internet + Mobile App | Global |
| Web LAN | BTH 7 | WebSocket (LAN) | Same WiFi network | Local LAN |
| **Web Serial & 3D** | **BTH 8** | **Web Serial / REST API** | **USB/WiFi + Browser** | **Local/LAN** |

### Stage-by-Stage Protocol Evolution

| Stage | Web Storage | WebSocket Protocol | New Feature |
| :---: | :--- | :--- | :--- |
| Giai doan 1 | PROGMEM (embedded in firmware) | Plain-text `M 0 90` | WiFi STA + WebSocket basics |
| Giai doan 2 | SPIFFS (separate filesystem) | Plain-text `M/A/H/T/S` | Full 6-joint Web GUI |
| Giai doan 3 | SPIFFS | **JSON** `{"cmd":"M","joint":0,"angle":90}` | WiFiManager + Pose Save/Load |

### WebSocket Commands (Stage 2 — Plain-text)

| Command | Syntax | Response | Description |
| :--- | :--- | :--- | :--- |
| **M** | `M <joint> <angle>` | `OK` | Move 1 joint |
| **A** | `A <a0> <a1> <a2> <a3> <a4> <a5>` | `OK` | Move all 6 joints simultaneously |
| **H** | `H` or `H <joint>` | `OK` | Home all or 1 joint |
| **T** | `T` | `STA:90,70,90,90,90,90` | Get current angles |

### WebSocket Commands (Stage 3 — JSON)

```json
// Client → ESP32
{"cmd": "M", "joint": 0, "angle": 90}
{"cmd": "A", "angles": [90, 70, 90, 90, 90, 90]}
{"cmd": "H"}
{"cmd": "T"}

// ESP32 → Client
{"ok": true, "joint": 0, "angle": 90}
{"ok": true, "angles": [90, 70, 90, 90, 90, 90]}
{"ok": false, "error": "out_of_range"}
```

### Usage — Stage 2 & 3

1. Change `WIFI_SSID` and `WIFI_PASS` in `main.cpp` (Stage 1-2) **or** leave blank for WiFiManager (Stage 3)
2. Build and upload firmware: click **Upload** in VS Code / PlatformIO
3. Upload web files (Stage 2-3 only): run `pio run -t uploadfs` in the terminal
4. Open Serial Monitor → note the IP address printed (e.g., `192.168.1.105`)
5. Open a browser on the **same WiFi network** → go to `http://192.168.1.105`

---

## 🚀 8. Web Serial & 3D Digital Twin (BTH 8)

Bai Thuc Hanh 8 introduces the most advanced Web capabilities without needing additional apps or desktop software:

1. **Web Serial API (Stage 1)**: Allows an HTML/JS file running in Google Chrome/Edge to directly connect to the ESP32 via USB. No Python, no drivers needed — just plug and play.
2. **3D Digital Twin (Stage 2)**: Uses **Three.js** to render a 3D model of the robotic arm directly in the browser. The 3D model moves exactly as the physical arm moves in real-time.
3. **Sequence Recorder (Teach & Play)**: Save sequence of poses and automatically play them back.

---

*This document is specially designed for teaching Embedded Systems & IoT. Good luck to all students!*

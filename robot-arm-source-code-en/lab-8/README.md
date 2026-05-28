# Lab 8: Digital Twin 3D & Web Serial API

## Overview

Lab 8 is the most advanced lab in this course. It introduces two cutting-edge web technologies for controlling the robotic arm:

| Stage | Topic | Key Technology | Connection |
|-------|-------|---------------|------------|
| Stage 1 | Web Serial API | Browser ↔ USB Serial | USB cable (no WiFi needed) |
| Stage 2 | 3D Digital Twin + REST API | Three.js + HTTP REST | USB (Serial) or WiFi (REST) |

## Prerequisites

- **Firmware**: Lab 4 Stage 3 firmware must be flashed on ESP32 (supports M, G, T, H, S, A, W commands)
- **Hardware**: ESP32 DevKit V1 + PCA9685 + 6-DOF Robot Arm
- **Browser**: Google Chrome 89+ or Microsoft Edge 89+ (required for Web Serial API)

## Stage 1: Web Serial API

Control the robot arm directly from a web browser via USB cable — no WiFi, no Python, no additional software needed.

### Files
```
stage-1/
├── web-serial-gui/
│   ├── index.html      ← Open this file in Chrome/Edge
│   ├── app.js           ← Web Serial logic + slider controls
│   └── style.css        ← Dark-mode UI styling
└── firmware/            ← (Uses Lab 4 Stage 3 firmware)
```

### Quick Start
1. Flash Lab 4 Stage 3 firmware onto ESP32
2. Close Serial Monitor in VS Code
3. Open `stage-1/web-serial-gui/index.html` in Chrome/Edge
4. Select baud rate 115200, click "Connect via USB"
5. Choose the ESP32 COM port → Start controlling!

## Stage 2: 3D Digital Twin + Sequence Recorder + REST API

Full-featured robot control interface with:
- **3D Digital Twin**: Real-time wireframe 3D model using Three.js
- **Dual Connection Mode**: Web Serial (USB) or REST API (WiFi)
- **Sequence Recorder**: Record poses and play them back (Teach & Play)
- **Mock Mode**: Test the UI without hardware

### Files
```
stage-2/
├── data/
│   ├── index.html       ← Full dashboard with 3D + controls
│   ├── app.js           ← Three.js 3D + Serial + REST + Sequence logic
│   ├── style.css        ← Premium dark-mode design
│   ├── API_SPECS.md     ← REST API documentation
│   └── CommandReference.md ← Serial command protocol
├── src/
│   ├── main.cpp         ← Entry point: Serial + WiFi + Web Server
│   ├── config.h         ← Hardware parameters (pins, limits, WiFi)
│   ├── pca9685.h/.cpp   ← PCA9685 I2C driver
│   ├── servo_ctrl.h/.cpp ← Servo motion controller
│   ├── cmd_parser.h/.cpp ← Serial command parser
│   └── web_server.h/.cpp ← WiFi AP/STA + REST API endpoints
└── platformio.ini       ← PlatformIO build configuration
```

### Quick Start (REST API mode)
1. Edit `src/config.h`: set `CFG_WIFI_SSID` and `CFG_WIFI_PASSWORD`
2. Build and flash: `pio run -t upload`
3. Upload web files to SPIFFS: `pio run -t uploadfs`
4. Open Serial Monitor → note the IP address
5. Open browser → enter IP → enjoy 3D Digital Twin!

### Quick Start (Web Serial mode)
1. Open `data/index.html` directly in Chrome/Edge
2. Select "🔌 Serial Port" mode
3. Click "Connect Device" → choose COM port
4. Control robot with sliders, watch 3D model sync in real-time

## Architecture Comparison

| Architecture | Lab | Protocol | Interface | Scope |
|---|---|---|---|---|
| Serial USB | Lab 4-5 | Plain-text Serial | Serial Monitor / Python GUI | Local (USB) |
| IoT Cloud | Lab 6 | MQTT/Blynk | Blynk Dashboard / Mobile | Global (Internet) |
| Web LAN | Lab 7 | WebSocket (LAN) | Web GUI 2D | Local network (WiFi) |
| Web Serial + 3D | Lab 8 | Web Serial / REST API | Web GUI 3D Digital Twin | Local (USB) / LAN (WiFi) |

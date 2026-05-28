# Stage 2: 3D Digital Twin + Sequence Recorder + REST API

## About Digital Twin

A **Digital Twin** is a virtual replica of a physical object synchronized in real-time. In this lab, the 3D robot arm on screen mirrors the physical robot — when you move sliders, both the 3D model and the real robot move simultaneously.

## Architecture

```
┌─────────────────────────────────────────────────────────┐
│                    Web Browser                           │
│  ┌──────────┐  ┌──────────┐  ┌────────────────────┐    │
│  │ Three.js │  │ Sliders  │  │ Sequence Recorder  │    │
│  │ 3D Model │  │ Controls │  │ (Teach & Play)     │    │
│  └────┬─────┘  └────┬─────┘  └────────┬───────────┘    │
│       │              │                  │                │
│       └──────────────┼──────────────────┘                │
│                      │                                   │
│    ┌─────────────────┼─────────────────┐                │
│    │  Connection Manager               │                │
│    │  ┌─────────┐   ┌────────────┐     │                │
│    │  │ Serial  │   │  REST API  │     │                │
│    │  │ (USB)   │   │  (WiFi)    │     │                │
│    │  └────┬────┘   └─────┬──────┘     │                │
│    └───────┼──────────────┼────────────┘                │
└────────────┼──────────────┼──────────────────────────────┘
             │              │
        USB Cable      WiFi LAN
             │              │
┌────────────┼──────────────┼──────────────────────────────┐
│    ESP32   │              │                              │
│    ┌───────┴─────┐  ┌─────┴──────┐                      │
│    │ Serial RX   │  │ Web Server │                      │
│    └──────┬──────┘  └─────┬──────┘                      │
│           │               │                              │
│    ┌──────┴───────────────┴──────┐                       │
│    │      Command Parser          │                      │
│    └──────────────┬───────────────┘                      │
│                   │                                      │
│    ┌──────────────┴───────────────┐                      │
│    │      Servo Controller        │                      │
│    │      (smooth motion)         │                      │
│    └──────────────┬───────────────┘                      │
│                   │                                      │
│    ┌──────────────┴───────────────┐                      │
│    │      PCA9685 (I2C PWM)       │                      │
│    └──────────────┬───────────────┘                      │
│                   │                                      │
└───────────────────┼──────────────────────────────────────┘
                    │ PWM signals
            ┌───────┴───────┐
            │  6 Servo Motors │
            └───────────────┘
```

## Quick Start (REST API mode)

1. Edit `src/config.h`: set WiFi credentials
2. Build firmware: `pio run -t upload`
3. Upload web files: `pio run -t uploadfs`
4. Open Serial Monitor → note the IP address
5. Open browser → enter IP → enjoy 3D Digital Twin!

## Quick Start (Web Serial mode)

1. Open `data/index.html` directly in Chrome/Edge
2. Select "🔌 Serial Port" mode
3. Click "Connect Device" → choose COM port
4. Control and watch the 3D model sync in real-time

## Features

### 3D Visualization (Three.js)
- Wireframe 3D robot arm model with kinematic chain
- J0 rotates Y-axis (base), J1-J3 rotate Z-axis (lift), J4 rotates Y-axis (wrist roll)
- J5 gripper: two fingers open/close based on angle
- Zoom In/Out controls

### Manual Controls
- 6 joint sliders with configurable min/max/home angles
- HOME ALL, STOP, SYNC buttons
- Settings modal for joint configuration

### Sequence Recorder (Teach & Play)
- **Add Frame**: capture current angles
- **Play**: execute recorded sequence (1 second per frame)
- **Stop**: interrupt playback
- Industrial teaching pendant principle

### Connection Modes
- **Serial Port**: Web Serial API via USB
- **REST API**: HTTP POST/GET via WiFi
- **Mock Mode**: auto-fallback when no device connected

## REST API Endpoints

| Method | Endpoint | Description |
|--------|----------|-------------|
| GET | `/api/status` | Get all servo angles and motion state |
| GET | `/api/info` | Get servo configuration (limits, defaults) |
| POST | `/api/move` | Move single servo: `id=0&angle=90` |
| POST | `/api/move_all` | Move all servos: `angles=90 70 90 90 90 90` |
| POST | `/api/home` | Home all or single servo |
| POST | `/api/stop` | Emergency stop |
| POST | `/api/speed` | Set servo speed: `id=0&speed=3` |
| POST | `/api/wait` | Wait until all servos reach target |

## Source Files

### Firmware (`src/`)

| File | Description |
|------|-------------|
| `main.cpp` | Entry point: Serial + WiFi + Web Server init |
| `config.h` | Hardware parameters: I2C pins, servo limits, WiFi |
| `pca9685.h/.cpp` | PCA9685 I2C PWM driver |
| `servo_ctrl.h/.cpp` | Smooth motion controller with speed control |
| `cmd_parser.h/.cpp` | Serial command parser (M, G, T, H, S, A, W, X, I) |
| `web_server.h/.cpp` | WiFi AP/STA + REST API endpoints |

### Web Interface (`data/`)

| File | Description |
|------|-------------|
| `index.html` | Full dashboard: 3D viewer + controls + recorder |
| `app.js` | Three.js 3D engine + Serial/REST + Sequence logic |
| `style.css` | Premium dark-mode UI with glowing effects |

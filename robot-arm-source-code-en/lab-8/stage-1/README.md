# Stage 1: Web Serial API — Control robot from browser via USB

## About Web Serial API

Web Serial API is a modern browser API that allows web pages to communicate directly with serial devices (COM ports) via USB. No additional software, Python, or drivers required.

**Supported browsers:** Google Chrome 89+, Microsoft Edge 89+

## How it works

```
┌──────────────────────┐    USB Cable    ┌──────────────────────┐
│   Web Browser        │ ◄──────────────►│   ESP32 + PCA9685    │
│   (Chrome/Edge)      │   Web Serial    │   + 6 Servos         │
│                      │   API (115200)  │                      │
│  ┌────────────────┐  │                 │  ┌────────────────┐  │
│  │  index.html    │  │   TX: "M 0 90"  │  │  Firmware      │  │
│  │  app.js        │  │   RX: "OK"      │  │  (Lab 4 GD3)   │  │
│  │  style.css     │  │                 │  │                │  │
│  └────────────────┘  │                 │  └────────────────┘  │
└──────────────────────┘                 └──────────────────────┘
```

## Quick Start

1. Flash **Lab 4 Stage 3** firmware onto ESP32
2. **Close** Serial Monitor in VS Code (COM port allows only 1 app)
3. Open `index.html` in **Chrome** or **Edge**
4. Select **115200 baud** → Click **Connect via USB**
5. Choose the ESP32 COM port → Done!

## Features

- 6 joint sliders (J0-J5) with real-time control
- HOME ALL: return all joints to default position
- STOP: emergency stop
- SYNC: read current angles from firmware (STA: command)
- Command Console: shows TX/RX log

## Files

| File | Description |
|------|-------------|
| `index.html` | Main HTML page with slider UI |
| `app.js` | Web Serial connection logic + slider event handlers |
| `style.css` | Dark-mode premium UI styling |
| `API_SPECS.md` | REST API specification (for Stage 2) |
| `CommandReference.md` | Serial command protocol reference |

## Note

No firmware changes needed — this stage uses the **existing firmware from Lab 4 Stage 3** which already supports the serial command protocol (M, G, T, H, S, A, W commands).

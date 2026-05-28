// ============================================================
// LAB 7 - STAGE 1: TRANG WEB NHUNG (PROGMEM)
// Description: Chua trang HTML dieu khien 1 servo (Base).
//        File nay duoc nhung thang vao firmware qua PROGMEM
//        → Khong can SPIFFS, khong can upload file rieng.
//        (STAGE 2 se chuyen sang SPIFFS cho linh hoat hon)
// ============================================================

#ifndef INDEX_HTML_H
#define INDEX_HTML_H

#include <Arduino.h>

// Chuoi HTML duoc Save trong Flash (PROGMEM) de tiet kiem RAM
// R"rawstring(...)rawstring" la cu phap C++11 "raw string literal"
// cho phep viet HTML nhieu dong ma khong can escape ky tu dac biet
const char INDEX_HTML[] PROGMEM = R"rawhtml(
<!DOCTYPE html>
<html lang="vi">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Robot Arm - Web Send (STAGE 1)</title>
  <style>
    /* === Interface CO BAN === */
    body {
      font-family: Arial, sans-serif;
      background: #1a1a2e;
      color: #e0e0e0;
      display: flex;
      flex-direction: column;
      align-items: center;
      padding: 30px 20px;
      margin: 0;
    }
    h1 { color: #00d4ff; margin-bottom: 5px; }
    .subtitle { color: #888; margin-bottom: 30px; font-size: 14px; }

    /* === equal TRANG THAI KET NOI === */
    #status-bar {
      padding: 8px 20px;
      border-radius: 20px;
      margin-bottom: 25px;
      font-weight: bold;
      background: #333;
    }
    #status-bar.connected    { background: #1b5e20; color: #69f0ae; }
    #status-bar.disconnected { background: #b71c1c; color: #ff8a80; }

    /* === CARD DIEU KHIEN SERVO === */
    .servo-card {
      background: #16213e;
      border: 1px solid #0f3460;
      border-radius: 12px;
      padding: 25px 30px;
      width: 100%;
      max-width: 480px;
      margin-bottom: 20px;
    }
    .servo-card h2 { margin: 0 0 15px; color: #00d4ff; font-size: 18px; }

    /* === Slider (SLIDER) === */
    .slider-row { display: flex; align-items: center; gap: 12px; }
    .slider-row label { width: 90px; font-size: 14px; color: #aaa; }
    .slider-row input[type=range] {
      flex: 1;
      accent-color: #00d4ff;
      height: 6px;
    }
    .angle-display {
      width: 50px;
      text-align: right;
      font-weight: bold;
      color: #00d4ff;
    }

    /* === Button CHUC NANG === */
    .btn-row { display: flex; gap: 10px; margin-top: 20px; }
    button {
      flex: 1;
      padding: 10px;
      border: none;
      border-radius: 8px;
      font-size: 14px;
      font-weight: bold;
      cursor: pointer;
      transition: opacity 0.2s;
    }
    button:hover { opacity: 0.85; }
    .btn-home   { background: #2e7d32; color: white; }
    .btn-stop   { background: #c62828; color: white; }

    /* === LOG PANEL === */
    #log {
      width: 100%;
      max-width: 480px;
      background: #0d0d1a;
      border: 1px solid #333;
      border-radius: 8px;
      padding: 10px 15px;
      font-family: monospace;
      font-size: 12px;
      color: #00ff88;
      height: 120px;
      overflow-y: auto;
    }
  </style>
</head>
<body>

  <h1>&#x1F916; Robot Arm - Web Send</h1>
  <p class="subtitle">LAB 7 - STAGE 1: WebSocket qua WiFi LAN</p>

  <!-- Trang thai ket noi WebSocket -->
  <div id="status-bar" class="disconnected">&#x26A1; Connecting...</div>

  <!-- Card dieu khien Servo 0 (Base) -->
  <div class="servo-card">
    <h2>Joint 0: Base (&#x1F4CD; Cleary nen)</h2>
    <div class="slider-row">
      <label>Goc quay:</label>
      <input type="range" id="slider-0" min="0" max="180" value="90"
             oninput="onSlider(0, this.value)">
      <span class="angle-display" id="angle-0">90&deg;</span>
    </div>
    <div class="btn-row">
      <button class="btn-home" onclick="sendHome()">&#x1F3E0; HOME</button>
      <button class="btn-stop" onclick="sendStop()">&#x26D4; STOP</button>
    </div>
  </div>

  <!-- Panel log lenh -->
  <div id="log">--- WebSocket not connected ---</div>

<script>
// ============================================================
// WEBSOCKET CLIENT - STAGE 1
// Description: Ket noi toi ESP32 qua WebSocket, Send lenh plain-text.
//        Protocol: "M <joint> <angle>" - giong Serial BTH 4-5
// ============================================================

// Lay IP address cua ESP32 tu URL trang web hien Load
// (Vi ESP32 la web server, nen IP cua server = IP cua ESP32)
const esp32Ip = window.location.hostname;
const wsUrl   = `ws://${esp32Ip}/ws`;

let ws        = null;     // Doi tuong WebSocket
let debounce  = null;     // Timer debounce cho slider

// --- Initialize ket noi WebSocket ---
function connect() {
  log(`Connecting toi ${wsUrl}...`);
  ws = new WebSocket(wsUrl);

  ws.onopen = () => {
    setStatus(true);
    log('Ket noi WebSocket successfully!');
  };

  ws.onclose = () => {
    setStatus(false);
    log('Ngat ket noi. Retry in 3 seconds...');
    setTimeout(connect, 3000);   // automatically ket noi lai
  };

  ws.onerror = (e) => {
    log('Loi WebSocket: ' + e.type);
  };

  ws.onmessage = (e) => {
    log('← ESP32: ' + e.data);   // Hien thi phan hoi tu ESP32
  };
}

// --- Send lenh den ESP32 ---
function send(cmd) {
  if (ws && ws.readyState === WebSocket.OPEN) {
    ws.send(cmd);
    log('→ Send: ' + cmd);
  } else {
    log('Chua ket noi!');
  }
}

// --- Xu ly keo slider (co debounce 100ms) ---
// Debounce: Tranh Send qua nhieu lenh khi keo slider lien tuc
function onSlider(joint, value) {
  document.getElementById(`angle-${joint}`).textContent = value + '°';
  clearTimeout(debounce);
  debounce = setTimeout(() => {
    send(`M ${joint} ${value}`);  // Protocol plain-text: "M 0 90"
  }, 100);
}

// --- Send lenh HOME (ve vi tri mac dinh) ---
function sendHome() {
  send('H');
  document.getElementById('slider-0').value = 90;
  document.getElementById('angle-0').textContent = '90°';
}

// --- Send lenh STOP emergency ---
function sendStop() {
  send('H');
  log('!!! EMERGENCY STOP !!!');
}

// --- update hien thi trang thai ket noi ---
function setStatus(connected) {
  const bar = document.getElementById('status-bar');
  if (connected) {
    bar.textContent = '✅ Connected WebSocket';
    bar.className   = 'connected';
  } else {
    bar.textContent = '❌ Disconnected';
    bar.className   = 'disconnected';
  }
}

// --- Ghi log vao panel ---
function log(msg) {
  const el = document.getElementById('log');
  const time = new Date().toLocaleTimeString();
  el.innerHTML += `[${time}] ${msg}\n`;
  el.scrollTop  = el.scrollHeight;  // automatically cuon xuong cuoi
}

// --- Bat dau ket noi ngay khi trang load xong ---
connect();
</script>
</body>
</html>
)rawhtml";

#endif

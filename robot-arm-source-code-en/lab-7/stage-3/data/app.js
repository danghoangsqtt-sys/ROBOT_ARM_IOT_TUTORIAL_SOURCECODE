// ============================================================
// LAB 7 - STAGE 3: WEBSOCKET CLIENT (app.js)
// Diem nang cap so voi STAGE 2:
//   1. Protocol JSON (instead of plain-text)
//   2. Auto-reconnect voi exponential backoff
//   3. Pose Save/Load qua REST API (/api/pose/save, /api/pose/load)
//
// Protocol JSON (so sanh):
//   STAGE 2:  "M 0 90"           → STAGE 3: {"cmd":"M","joint":0,"angle":90}
//   STAGE 2:  "H"                 → STAGE 3: {"cmd":"H"}
//   STAGE 2:  "T"                 → STAGE 3: {"cmd":"T"}
//   STAGE 2:  "STA:90,70,..."     → STAGE 3: {"ok":true,"angles":[90,70,...]}
// ============================================================

// === CAU HINH Joint ===
const JOINTS = [
  { id: 0, name: 'Joint 0: Base',        min: 0,  max: 180, home: 90 },
  { id: 1, name: 'Joint 1: Shoulder',    min: 70, max: 150, home: 70 },
  { id: 2, name: 'Joint 2: Elbow',       min: 0,  max: 150, home: 90 },
  { id: 3, name: 'Joint 3: Wrist Pitch', min: 0,  max: 180, home: 90 },
  { id: 4, name: 'Joint 4: Wrist Roll',  min: 0,  max: 180, home: 90 },
  { id: 5, name: 'Joint 5: Gripper',     min: 60, max: 120, home: 90 }
];

// === BIEN TOAN CUC ===
let ws            = null;
let debounces     = {};
let angles        = JOINTS.map(j => j.home);
let reconnectDelay = 1000;    // Bat dau 1 seconds, tang dan (exponential backoff)
let reconnectTimer = null;

// === Initialize Interface ===
function buildUI() {
  // 6 Servo Cards (giong STAGE 2)
  const grid = document.getElementById('servo-grid');
  grid.innerHTML = JOINTS.map(j => `
    <div class="servo-card" id="card-${j.id}">
      <div class="servo-card-header">
        <span class="servo-name">${j.name}</span>
        <span class="servo-index">Channel ${j.id}</span>
      </div>
      <div class="slider-wrap">
        <input type="range" id="slider-${j.id}"
               min="${j.min}" max="${j.max}" value="${j.home}"
               oninput="onSlider(${j.id}, this.value)">
        <span class="angle-val" id="angle-${j.id}">${j.home}&deg;</span>
      </div>
      <div class="range-labels"><span>${j.min}&deg;</span><span>${j.max}&deg;</span></div>
      <button class="btn-joint-home" onclick="sendJointHome(${j.id})">
        &#x1F3E0; HOME Joint ${j.id}
      </button>
    </div>
  `).join('');

  // 5 Pose Slots (tinh nang moi STAGE 3)
  const poseSlots = document.getElementById('pose-slots');
  poseSlots.innerHTML = Array.from({length: 5}, (_, i) => `
    <div class="pose-slot">
      <span class="pose-slot-label">Pose ${i}</span>
      <button class="btn-pose-save" onclick="poseSave(${i})">&#x1F4BE; Save</button>
      <button class="btn-pose-load" onclick="poseLoad(${i})">&#x25B6; Load</button>
    </div>
  `).join('');
}

// === WEBSOCKET VỚI EXPONENTIAL BACKOFF ===
const ESP32_IP = window.location.hostname;
const WS_URL   = `ws://${ESP32_IP}/ws`;

function connect() {
  clearTimeout(reconnectTimer);
  log(`Connecting toi ${WS_URL}...`, 'info');
  ws = new WebSocket(WS_URL);

  ws.onopen = () => {
    setStatus(true);
    reconnectDelay = 1000;  // Reset delay khi Connection successful
    log('WebSocket: Connection successful!', 'info');
    sendJson({ cmd: 'T' });  // Lay trang thai hien Load
  };

  ws.onclose = () => {
    setStatus(false);
    log(`Disconnected. Retry in ${reconnectDelay/1000}s...`, 'error');
    reconnectTimer = setTimeout(() => {
      connect();
      // Exponential backoff: 1s → 2s → 4s → 8s → toi da 30s
      reconnectDelay = Math.min(reconnectDelay * 2, 30000);
    }, reconnectDelay);
  };

  ws.onerror = () => {
    log('WebSocket: Connection error', 'error');
  };

  ws.onmessage = (e) => {
    try {
      const data = JSON.parse(e.data);
      log(`← ESP32: ${e.data}`, 'recv');
      handleResponse(data);
    } catch {
      log(`← ESP32 (raw): ${e.data}`, 'recv');
    }
  };
}

// Send lenh JSON len ESP32
function sendJson(obj) {
  if (ws && ws.readyState === WebSocket.OPEN) {
    const msg = JSON.stringify(obj);
    ws.send(msg);
    log(`→ Send: ${msg}`, 'send');
  } else {
    log('WebSocket not connected!', 'error');
  }
}

// Xu ly phan hoi JSON tu ESP32
function handleResponse(data) {
  if (Array.isArray(data.angles) && data.angles.length === 6) {
    // update UI khi Receive duoc trang thai angles
    data.angles.forEach((angle, i) => {
      if (i < JOINTS.length) {
        angles[i] = angle;
        updateSliderUI(i, angle);
      }
    });
  }
}

// === XU LY SLIDER ===
function onSlider(jointId, value) {
  const angle = parseInt(value);
  angles[jointId] = angle;
  updateAngleDisplay(jointId, angle);
  clearTimeout(debounces[jointId]);
  debounces[jointId] = setTimeout(() => {
    // Protocol JSON (STAGE 3) instead of plain-text (STAGE 2)
    sendJson({ cmd: 'M', joint: jointId, angle: angle });
  }, 100);
}

function updateAngleDisplay(jointId, angle) {
  const el = document.getElementById(`angle-${jointId}`);
  if (el) el.textContent = `${angle}°`;
}

function updateSliderUI(jointId, angle) {
  updateAngleDisplay(jointId, angle);
  const slider = document.getElementById(`slider-${jointId}`);
  if (slider) slider.value = angle;
}

// === Button CHUC NANG ===
function sendJointHome(jointId) {
  sendJson({ cmd: 'H', joint: jointId });
  angles[jointId] = JOINTS[jointId].home;
  updateSliderUI(jointId, JOINTS[jointId].home);
}

function sendHome() {
  sendJson({ cmd: 'H' });
  JOINTS.forEach(j => { angles[j.id] = j.home; updateSliderUI(j.id, j.home); });
}

function sendStatus() {
  sendJson({ cmd: 'T' });
}

function sendStop() {
  sendJson({ cmd: 'H' });
  log('!!! EMERGENCY STOP !!!', 'error');
}

// === POSE SAVE / LOAD ===

// Save pose hien Load vao slot (qua REST API)
async function poseSave(slot) {
  try {
    const resp = await fetch('/api/pose/save', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ slot })
    });
    const data = await resp.json();
    if (data.ok) {
      log(`[Pose] Da Save Pose ${slot}`, 'info');
    } else {
      log(`[Pose] Loi Save: ${data.error}`, 'error');
    }
  } catch (e) {
    log(`[Pose] Cannot connect API: ${e}`, 'error');
  }
}

// Load pose tu slot va update UI
async function poseLoad(slot) {
  try {
    const resp = await fetch('/api/pose/load', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ slot })
    });
    const data = await resp.json();
    if (data.ok && data.angles) {
      data.angles.forEach((angle, i) => {
        angles[i] = angle;
        updateSliderUI(i, angle);
      });
      log(`[Pose] Da Load Pose ${slot}: ${data.angles.join(', ')}`, 'info');
    } else {
      log(`[Pose] Slot ${slot} no data`, 'error');
    }
  } catch (e) {
    log(`[Pose] Cannot connect API: ${e}`, 'error');
  }
}

// === LOG & STATUS ===
function log(msg, type = 'log') {
  const panel = document.getElementById('log-panel');
  if (!panel) return;
  const time = new Date().toLocaleTimeString('vi-VN');
  const line = document.createElement('div');
  line.className = `log-line ${type}`;
  line.textContent = `[${time}] ${msg}`;
  panel.appendChild(line);
  panel.scrollTop = panel.scrollHeight;
  while (panel.children.length > 200) panel.removeChild(panel.firstChild);
}

function clearLog() {
  const p = document.getElementById('log-panel');
  if (p) p.innerHTML = '';
}

function setStatus(connected) {
  const badge = document.getElementById('ws-status');
  const label = document.getElementById('ws-label');
  if (badge && label) {
    badge.className = `badge ${connected ? 'connected' : 'disconnected'}`;
    label.textContent = connected ? 'Connected' : 'Disconnected';
  }
}

// === boot ===
window.onload = () => {
  buildUI();
  connect();
};

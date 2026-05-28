// ============================================================
// LAB 7 - STAGE 2: WEBSOCKET CLIENT (app.js)
// Description: Xu ly WebSocket, tao Interface dong, Send/Receive lenh.
//        Protocol: plain-text (M, A, H, T) - same as LAB 4-5 Serial
//        (STAGE 3 se nang cap len JSON)
// ============================================================

// === CAU HINH CAC Joint ===
// Phai Joint voi cau hinh JOINTS[] trong servo_ctrl.h cua firmware
const JOINTS = [
  { id: 0, name: 'Joint 0: Base',        min: 0,  max: 180, home: 90 },
  { id: 1, name: 'Joint 1: Shoulder',    min: 70, max: 150, home: 70 },
  { id: 2, name: 'Joint 2: Elbow',       min: 0,  max: 150, home: 90 },
  { id: 3, name: 'Joint 3: Wrist Pitch', min: 0,  max: 180, home: 90 },
  { id: 4, name: 'Joint 4: Wrist Roll',  min: 0,  max: 180, home: 90 },
  { id: 5, name: 'Joint 5: Gripper',     min: 60, max: 120, home: 90 }
];

// === BIEN TOAN CUC ===
let ws        = null;           // Doi tuong WebSocket
let debounces = {};             // Map timer debounce moi Joint
let angles    = JOINTS.map(j => j.home);  // Goc hien Load cua 6 Joint

// === Initialize Interface ===
// Tao 6 servo card tu mang JOINTS (instead of viet cung trong HTML)
function buildUI() {
  const grid = document.getElementById('servo-grid');
  grid.innerHTML = JOINTS.map(j => `
    <div class="servo-card" id="card-${j.id}">
      <div class="servo-card-header">
        <span class="servo-name">${j.name}</span>
        <span class="servo-index">Channel ${j.id}</span>
      </div>

      <div class="slider-wrap">
        <input type="range"
               id="slider-${j.id}"
               min="${j.min}" max="${j.max}" value="${j.home}"
               oninput="onSlider(${j.id}, this.value)">
        <span class="angle-val" id="angle-${j.id}">${j.home}&deg;</span>
      </div>
      <div class="range-labels">
        <span>${j.min}&deg;</span>
        <span>${j.max}&deg;</span>
      </div>

      <button class="btn-joint-home"
              onclick="sendJointHome(${j.id})">
        &#x1F3E0; HOME Joint ${j.id}
      </button>
    </div>
  `).join('');

  log('Interface da Initialize. Connecting WebSocket...', 'info');
}

// === WEBSOCKET ===

// Lay IP address tu URL (ESP32 la web server nen IP trung voi hostname)
const ESP32_IP = window.location.hostname;
const WS_URL   = `ws://${ESP32_IP}/ws`;

function connect() {
  log(`Connecting toi ${WS_URL}...`, 'info');
  ws = new WebSocket(WS_URL);

  ws.onopen = () => {
    setStatus(true);
    log('WebSocket: Connection successful!', 'info');
    sendCmd('T');   // Request status hien Load khi vua ket noi
  };

  ws.onclose = () => {
    setStatus(false);
    log('WebSocket: Disconnected. Retry in 3 seconds...', 'error');
    setTimeout(connect, 3000);
  };

  ws.onerror = () => {
    log('WebSocket: Connection error', 'error');
  };

  ws.onmessage = (e) => {
    const msg = e.data;
    log(`← ESP32: ${msg}`, 'recv');
    handleResponse(msg);
  };
}

// Send lenh len ESP32 qua WebSocket
function sendCmd(cmd) {
  if (ws && ws.readyState === WebSocket.OPEN) {
    ws.send(cmd);
    log(`→ Send:  ${cmd}`, 'send');
  } else {
    log('WebSocket not connected!', 'error');
  }
}

// Xu ly phan hoi tu ESP32
// Protocol plain-text: "STA:90,70,90,90,90,90" hoac "OK"
function handleResponse(msg) {
  if (msg.startsWith('STA:')) {
    // update goc hien Load tu chuoi trang thai "STA:a0,a1,...,a5"
    const parts = msg.substring(4).split(',');
    parts.forEach((v, i) => {
      const angle = parseInt(v);
      if (!isNaN(angle) && i < JOINTS.length) {
        angles[i] = angle;
        updateSliderUI(i, angle);
      }
    });
  }
  // "OK" khong can xu ly them
}

// === XU LY SLIDER ===

// Khi keo slider: update hien thi, debounce 100ms roi Send lenh
function onSlider(jointId, value) {
  const angle = parseInt(value);
  angles[jointId] = angle;
  updateAngleDisplay(jointId, angle);

  // Debounce: tranh Send qua nhieu lenh khi keo Receiveh
  clearTimeout(debounces[jointId]);
  debounces[jointId] = setTimeout(() => {
    sendCmd(`M ${jointId} ${angle}`);
  }, 100);
}

// update so hien thi goc (khong di chuyen slider)
function updateAngleDisplay(jointId, angle) {
  const el = document.getElementById(`angle-${jointId}`);
  if (el) el.textContent = `${angle}°`;
}

// update ca slider va so hien thi (dung khi Receive STATE tu ESP32)
function updateSliderUI(jointId, angle) {
  updateAngleDisplay(jointId, angle);
  const slider = document.getElementById(`slider-${jointId}`);
  if (slider) slider.value = angle;
}

// === Button CHUC NANG ===

// HOME tung Joint rieng le: "H <joint>"
function sendJointHome(jointId) {
  sendCmd(`H ${jointId}`);
  const homeAngle = JOINTS[jointId].home;
  angles[jointId] = homeAngle;
  updateSliderUI(jointId, homeAngle);
}

// HOME All Joint: "H"
function sendHome() {
  sendCmd('H');
  JOINTS.forEach(j => {
    angles[j.id] = j.home;
    updateSliderUI(j.id, j.home);
  });
}

// Lay trang thai hien Load tu ESP32: "T"
function sendStatus() {
  sendCmd('T');
}

// EMERGENCY STOP: Send H emergency
function sendStop() {
  sendCmd('H');
  log('!!! EMERGENCY STOP: Da Send lenh HOME emergency !!!', 'error');
}

// === LOG PANEL ===

function log(msg, type = 'log') {
  const panel = document.getElementById('log-panel');
  if (!panel) return;

  const time = new Date().toLocaleTimeString('vi-VN');
  const line = document.createElement('div');
  line.className = `log-line ${type}`;
  line.textContent = `[${time}] ${msg}`;
  panel.appendChild(line);
  panel.scrollTop = panel.scrollHeight;

  // Gioi han toi da 200 dong log
  while (panel.children.length > 200) {
    panel.removeChild(panel.firstChild);
  }
}

function clearLog() {
  const panel = document.getElementById('log-panel');
  if (panel) panel.innerHTML = '';
}

// === update TRANG THAI KET NOI ===
function setStatus(connected) {
  const badge = document.getElementById('ws-status');
  const label = document.getElementById('ws-label');
  if (badge && label) {
    badge.className = `badge ${connected ? 'connected' : 'disconnected'}`;
    label.textContent = connected ? 'Connected' : 'Disconnected';
  }
}

// === boot KHI TRANG LOAD ===
window.onload = () => {
  buildUI();    // Tao Interface 6 slider
  connect();    // Bat dau ket noi WebSocket
};

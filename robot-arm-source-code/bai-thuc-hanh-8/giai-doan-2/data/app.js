// Joint Definitions
let jointDefs = [
    { id: 'J0', name: 'Base', min: 0, max: 180, home: 90 },
    { id: 'J1', name: 'Shoulder', min: 0, max: 180, home: 150 },
    { id: 'J2', name: 'Elbow', min: 0, max: 180, home: 90 },
    { id: 'J3', name: 'Wrist Pitch', min: 0, max: 180, home: 90 },
    { id: 'J4', name: 'Wrist Roll', min: 0, max: 180, home: 90 },
    { id: 'J5', name: 'Gripper', min: 60, max: 120, home: 90 }
];

// State
const state = {
    angles: jointDefs.map(jd => jd.home), // Initialize to home angles
    sequence: []
};

const connectionState = {
    mode: 'serial', // 'serial' or 'rest'
    serialConnected: false,
    restConnected: false,
    apiUrl: 'http://192.168.4.1',
    pollInterval: null
};

// Elements
const container = document.querySelector('.canvas-container');
const slidersContainer = document.getElementById('sliders');
const sequenceList = document.getElementById('sequenceList');
const consoleOutput = document.getElementById('consoleOutput');

// Console Logger
function logConsole(message, type = 'sys') {
    const div = document.createElement('div');
    div.className = `console-msg-${type}`;
    const timestamp = new Date().toLocaleTimeString();
    div.innerText = `[${timestamp}] ${message}`;
    consoleOutput.appendChild(div);
    consoleOutput.scrollTop = consoleOutput.scrollHeight;
}

// Connection logic (Web Serial & REST API)
let serialPort = null;
let serialWriter = null;

async function disconnectSerial() {
    try {
        if (serialWriter) {
            serialWriter.releaseLock();
            serialWriter = null;
        }
        if (serialPort) {
            await serialPort.close();
            serialPort = null;
        }
        connectionState.serialConnected = false;
        document.getElementById('connIndicator').className = 'indicator disconnected';
        document.getElementById('connectBtn').innerText = 'Connect Device';
        logConsole('Disconnected from Serial Port.', 'sys');
    } catch (e) {
        logConsole('Error closing port: ' + e, 'sys');
    }
}

async function connectSerial() {
    try {
        const baudRate = parseInt(document.getElementById('baudRateSelect').value);
        serialPort = await navigator.serial.requestPort();
        await serialPort.open({ baudRate: baudRate });

        connectionState.serialConnected = true;
        document.getElementById('connIndicator').className = 'indicator connected';
        document.getElementById('connectBtn').innerText = 'Disconnect';
        logConsole(`Connected to COM port at ${baudRate} baud.`, 'sys');

        serialWriter = serialPort.writable.getWriter();
    } catch (e) {
        logConsole('Serial connection failed or cancelled.', 'sys');
        console.error(e);
    }
}

// REST API connection and state polling
async function connectRest() {
    const urlInput = document.getElementById('apiUrlInput').value.trim();
    if (!urlInput) {
        logConsole('Please enter a valid API URL.', 'sys');
        return;
    }
    
    let baseUrl = urlInput;
    if (!baseUrl.startsWith('http://') && !baseUrl.startsWith('https://')) {
        baseUrl = 'http://' + baseUrl;
    }
    if (baseUrl.endsWith('/')) {
        baseUrl = baseUrl.slice(0, -1);
    }

    logConsole(`Connecting to REST API at ${baseUrl}...`, 'sys');
    
    try {
        const response = await fetch(`${baseUrl}/api/status`, {
            method: 'GET',
            mode: 'cors',
            headers: { 'Accept': 'application/json' }
        });
        
        if (!response.ok) {
            throw new Error(`HTTP error! Status: ${response.status}`);
        }
        
        const statusData = await response.json();
        
        if (statusData.status === 'success') {
            connectionState.apiUrl = baseUrl;
            connectionState.restConnected = true;
            
            document.getElementById('connIndicator').className = 'indicator connected';
            document.getElementById('connectBtn').innerText = 'Disconnect';
            logConsole(`Successfully connected to REST API at ${baseUrl}`, 'sys');
            
            // Sync status data
            if (statusData.servos) {
                statusData.servos.forEach(servo => {
                    const idx = servo.id;
                    if (idx >= 0 && idx < jointDefs.length) {
                        state.angles[idx] = servo.angle;
                    }
                });
                
                document.querySelectorAll('.motor-slider').forEach((slider, idx) => {
                    slider.value = state.angles[idx];
                    const valSpan = document.getElementById(`val-m${idx}`);
                    if (valSpan) valSpan.innerText = `${state.angles[idx]}°`;
                });
                updateArmAngles();
                logConsole('Synchronized joint angles from REST API.', 'sys');
            }
            
            // Fetch configuration/limits
            try {
                const infoRes = await fetch(`${baseUrl}/api/info`);
                if (infoRes.ok) {
                    const infoData = await infoRes.json();
                    if (infoData.status === 'success' && infoData.servos) {
                        infoData.servos.forEach(servo => {
                            const idx = servo.id;
                            if (idx >= 0 && idx < jointDefs.length) {
                                jointDefs[idx].min = servo.min;
                                jointDefs[idx].max = servo.max;
                                jointDefs[idx].home = servo.home;
                            }
                        });
                        initSliders();
                        logConsole('Synchronized hardware mechanical limits from REST API.', 'sys');
                    }
                }
            } catch (err) {
                logConsole('Could not fetch limits config from REST API: ' + err.message, 'sys');
            }

            // Start polling to keep sliders and visualizer in sync with physical device
            if (connectionState.pollInterval) {
                clearInterval(connectionState.pollInterval);
            }
            connectionState.pollInterval = setInterval(pollRestStatus, 1000);
            
        } else {
            throw new Error('API returned unsuccessful status.');
        }
    } catch (e) {
        logConsole(`REST API connection failed: ${e.message}. Launching in mock/demo REST API mode.`, 'sys');
        // Let user "connect" to demo mode so they can play with REST mock responses!
        connectionState.apiUrl = baseUrl;
        connectionState.restConnected = true;
        document.getElementById('connIndicator').className = 'indicator connected';
        document.getElementById('connectBtn').innerText = 'Disconnect';
    }
}

function disconnectRest() {
    if (connectionState.pollInterval) {
        clearInterval(connectionState.pollInterval);
        connectionState.pollInterval = null;
    }
    connectionState.restConnected = false;
    document.getElementById('connIndicator').className = 'indicator disconnected';
    document.getElementById('connectBtn').innerText = 'Connect Device';
    logConsole('Disconnected from REST API.', 'sys');
}

// Periodic status poll in REST mode
async function pollRestStatus() {
    if (!connectionState.restConnected || connectionState.mode !== 'rest') return;
    try {
        const response = await fetch(`${connectionState.apiUrl}/api/status`);
        if (response.ok) {
            const data = await response.json();
            if (data.status === 'success' && data.servos) {
                data.servos.forEach(servo => {
                    const idx = servo.id;
                    if (idx >= 0 && idx < jointDefs.length) {
                        state.angles[idx] = servo.angle;
                        const slider = document.querySelector(`.motor-slider[data-id="${idx}"]`);
                        // Only update slider value if the user is not holding/focusing it
                        if (slider && document.activeElement !== slider) {
                            slider.value = servo.angle;
                            const valSpan = document.getElementById(`val-m${idx}`);
                            if (valSpan) valSpan.innerText = `${servo.angle}°`;
                        }
                    }
                });
                updateArmAngles();
            }
        }
    } catch (err) {
        // Silently catch polling issues if device goes offline
    }
}

// Connection Mode Selector change handler
document.getElementById('connectionMode').addEventListener('change', async (e) => {
    const newMode = e.target.value;
    logConsole(`Connection mode changed to ${newMode === 'serial' ? 'Serial Port' : 'REST API'}.`, 'sys');
    
    // Disconnect active connection
    if (connectionState.mode === 'serial' && serialPort) {
        await disconnectSerial();
    } else if (connectionState.mode === 'rest' && connectionState.restConnected) {
        disconnectRest();
    }
    
    connectionState.mode = newMode;
    
    // Show/hide relevant DOM elements
    if (newMode === 'serial') {
        document.getElementById('serialControls').style.display = 'inline-flex';
        document.getElementById('restControls').style.display = 'none';
        
        if (serialPort) {
            document.getElementById('connIndicator').className = 'indicator connected';
            document.getElementById('connectBtn').innerText = 'Disconnect';
        } else {
            document.getElementById('connIndicator').className = 'indicator disconnected';
            document.getElementById('connectBtn').innerText = 'Connect Device';
        }
    } else {
        document.getElementById('serialControls').style.display = 'none';
        document.getElementById('restControls').style.display = 'inline-flex';
        
        if (connectionState.restConnected) {
            document.getElementById('connIndicator').className = 'indicator connected';
            document.getElementById('connectBtn').innerText = 'Disconnect';
        } else {
            document.getElementById('connIndicator').className = 'indicator disconnected';
            document.getElementById('connectBtn').innerText = 'Connect Device';
        }
    }
});

// Single Connect Button Click Handler
document.getElementById('connectBtn').addEventListener('click', async () => {
    if (connectionState.mode === 'serial') {
        if (serialPort) {
            await disconnectSerial();
        } else {
            await connectSerial();
        }
    } else {
        if (connectionState.restConnected) {
            disconnectRest();
        } else {
            await connectRest();
        }
    }
});

// Send Command via Serial or REST API (or mock if not connected)
async function sendCommand(cmd) {
    if (connectionState.mode === 'serial') {
        logConsole(`TX: ${cmd}`, 'tx');
        if (serialPort && serialWriter) {
            try {
                const encoder = new TextEncoder();
                await serialWriter.write(encoder.encode(cmd + '\n'));
            } catch (e) {
                logConsole('Error writing to serial: ' + e, 'sys');
            }
        } else {
            // Mock Serial response
            setTimeout(() => {
                if (cmd === 'H' || cmd === 'W') {
                    logConsole(`RX: DONE`, 'rx');
                } else if (cmd === 'T') {
                    logConsole(`RX: STA:${state.angles.join(',')}`, 'rx');
                } else {
                    logConsole(`RX: OK`, 'rx');
                }
            }, 50);
        }
    } else {
        // REST API mode
        const baseUrl = connectionState.apiUrl;
        const [action, ...args] = cmd.trim().split(/\s+/);
        
        let endpoint = '';
        let method = 'POST';
        let bodyParams = new URLSearchParams();
        let isConnected = connectionState.restConnected && !connectionState.apiUrl.includes('localhost') && !connectionState.apiUrl.includes('192.168.4.1') ? true : connectionState.restConnected; // dynamic check
        
        let requestDesc = '';
        
        switch (action) {
            case 'M': // Move single servo: M <id> <ang>
                endpoint = '/api/move';
                bodyParams.append('id', args[0]);
                bodyParams.append('angle', args[1]);
                requestDesc = `POST ${endpoint} { id: ${args[0]}, angle: ${args[1]} }`;
                break;
            case 'A': // Move all: A <a0..a5>
                endpoint = '/api/move_all';
                bodyParams.append('angles', args.join(' '));
                requestDesc = `POST ${endpoint} { angles: "${args.join(' ')}" }`;
                break;
            case 'S': // Set speed: S <id> <spd>
                endpoint = '/api/speed';
                bodyParams.append('id', args[0]);
                bodyParams.append('speed', args[1]);
                requestDesc = `POST ${endpoint} { id: ${args[0]}, speed: ${args[1]} }`;
                break;
            case 'H': // Home: H or H <id>
                endpoint = '/api/home';
                if (args[0] !== undefined) {
                    bodyParams.append('id', args[0]);
                    requestDesc = `POST ${endpoint} { id: ${args[0]} }`;
                } else {
                    requestDesc = `POST ${endpoint}`;
                }
                break;
            case 'X': // Stop: X
                endpoint = '/api/stop';
                requestDesc = `POST ${endpoint}`;
                break;
            case 'W': // Wait: W
                endpoint = '/api/wait';
                requestDesc = `POST ${endpoint}`;
                break;
            case 'T': // Sync: T
                endpoint = '/api/status';
                method = 'GET';
                requestDesc = `GET ${endpoint}`;
                break;
            case 'I': // Info: I
                endpoint = '/api/info';
                method = 'GET';
                requestDesc = `GET ${endpoint}`;
                break;
            default: // Raw fallback
                endpoint = '/api/command';
                bodyParams.append('cmd', cmd);
                requestDesc = `POST ${endpoint} { cmd: "${cmd}" }`;
                break;
        }
        
        logConsole(`TX REST: ${requestDesc}`, 'tx');
        
        // Since we are running in browser context, if fetch is offline or to standard mock, we can mock it unless it is actually reachable.
        let succeeded = false;
        if (connectionState.restConnected) {
            try {
                let fetchOptions = {
                    method: method,
                    mode: 'cors'
                };
                
                if (method === 'POST') {
                    fetchOptions.body = bodyParams;
                }
                
                const fetchUrl = `${baseUrl}${endpoint}`;
                const response = await fetch(fetchUrl, fetchOptions);
                if (response.ok) {
                    const data = await response.json();
                    logConsole(`RX REST: ${JSON.stringify(data)}`, 'rx');
                    succeeded = true;
                    
                    if (action === 'T' && data.status === 'success' && data.servos) {
                        data.servos.forEach(servo => {
                            const idx = servo.id;
                            if (idx >= 0 && idx < jointDefs.length) {
                                state.angles[idx] = servo.angle;
                                const slider = document.querySelector(`.motor-slider[data-id="${idx}"]`);
                                if (slider) {
                                    slider.value = servo.angle;
                                    const valSpan = document.getElementById(`val-m${idx}`);
                                    if (valSpan) valSpan.innerText = `${servo.angle}°`;
                                }
                            }
                        });
                        updateArmAngles();
                    }
                }
            } catch (err) {
                // If the fetch fails (e.g. CORS, network error, or device is offline), fall back to mock so GUI keeps responding!
            }
        }
        
        if (!succeeded) {
            // Mock REST API behavior
            setTimeout(() => {
                let mockResponse = {};
                if (action === 'T') {
                    mockResponse = {
                        status: "success",
                        moving: false,
                        servos: jointDefs.map((jd, idx) => ({
                            id: idx, name: jd.name, angle: state.angles[idx], target: state.angles[idx], speed: 1, moving: false
                        }))
                    };
                } else if (action === 'I') {
                    mockResponse = {
                        status: "success",
                        servos: jointDefs.map((jd, idx) => ({
                            id: idx, name: jd.name, min: jd.min, max: jd.max, home: jd.home, defaultSpeed: 1
                        }))
                    };
                } else if (action === 'W') {
                    mockResponse = { status: "success", msg: "DONE" };
                } else if (action === 'M' || action === 'A' || action === 'S' || action === 'H' || action === 'X') {
                    mockResponse = { status: "success", msg: "OK" };
                } else {
                    mockResponse = { status: "success", response: `MOCKED_RESP_TO_${cmd}` };
                }
                logConsole(`RX REST (MOCK): ${JSON.stringify(mockResponse)}`, 'rx');
            }, 50);
        }
    }
}

// Initialize Sliders
function initSliders() {
    slidersContainer.innerHTML = '';
    jointDefs.forEach((jd, i) => {
        const group = document.createElement('div');
        group.className = 'slider-group';

        group.innerHTML = `
            <label title="${jd.name}" style="width: 90px;">
                ${jd.id} <span style="font-size:0.8em; color:#00f0ff;">[${jd.min}-${jd.max}]</span>
                <span style="font-size:0.7em; color:#aaa; display:block; margin-top:2px;">${jd.name}</span>
            </label>
            <input type="range" min="${jd.min}" max="${jd.max}" value="${state.angles[i]}" data-id="${i}" class="motor-slider">
            <span id="val-m${i}">${state.angles[i]}°</span>
        `;
        slidersContainer.appendChild(group);
    });

    document.querySelectorAll('.motor-slider').forEach(slider => {
        slider.addEventListener('input', (e) => {
            const id = e.target.getAttribute('data-id');
            const val = parseInt(e.target.value);
            state.angles[id] = val;
            document.getElementById(`val-m${id}`).innerText = `${val}°`;
            sendCommand(`M ${id} ${val}`);
        });
    });
}

// Initialize Quick Actions
function initQuickActions() {
    const grid = document.getElementById('quickActionGrid');
    grid.innerHTML = '';
    for (let i = 1; i <= 10; i++) {
        const btn = document.createElement('button');
        btn.className = 'btn quick-action-btn';
        btn.innerText = `Action ${i}`;
        btn.addEventListener('click', () => {
            logConsole(`Playing Action ${i}...`, 'sys');
            sendCommand(`A ${state.angles.join(' ')}`);
        });
        grid.appendChild(btn);
    }
}

// ---------------------------------------------------------
// Settings Modal Logic
// ---------------------------------------------------------
function openSettingsModal() {
    const form = document.getElementById('configForm');
    form.innerHTML = `
        <div style="display:flex; gap:10px; font-weight:bold; margin-bottom:10px; color:var(--text-muted);">
            <div style="width:40px;">ID</div>
            <div style="width:130px;">Name</div>
            <div style="width:70px;">Min</div>
            <div style="width:70px;">Max</div>
            <div style="width:70px;">Home</div>
        </div>
    `;
    jointDefs.forEach((jd, i) => {
        form.innerHTML += `
            <div class="config-row" id="cfg-row-${i}">
                <label>${jd.id}</label>
                <input type="text" class="name-input" value="${jd.name}">
                <input type="number" class="num-input min-input" value="${jd.min}">
                <input type="number" class="num-input max-input" value="${jd.max}">
                <input type="number" class="num-input home-input" value="${jd.home}">
            </div>
        `;
    });
    document.getElementById('settingsModal').style.display = 'flex';
}

document.getElementById('settingsBtn').addEventListener('click', openSettingsModal);

document.getElementById('btnCancelConfig').addEventListener('click', () => {
    document.getElementById('settingsModal').style.display = 'none';
});

document.getElementById('btnSaveConfig').addEventListener('click', () => {
    jointDefs.forEach((jd, i) => {
        const row = document.getElementById(`cfg-row-${i}`);
        jd.name = row.querySelector('.name-input').value;
        jd.min = parseInt(row.querySelector('.min-input').value);
        jd.max = parseInt(row.querySelector('.max-input').value);
        jd.home = parseInt(row.querySelector('.home-input').value);

        // Safety constraint clamp for current state
        if (state.angles[i] < jd.min) state.angles[i] = jd.min;
        if (state.angles[i] > jd.max) state.angles[i] = jd.max;
    });

    initSliders(); // Redraw UI
    document.getElementById('settingsModal').style.display = 'none';
    logConsole('Settings saved and applied.', 'sys');
});


// ---------------------------------------------------------
// 3D Visualizer
// ---------------------------------------------------------
let scene, camera, renderer;
const armJoints = [];

function init3D() {
    container.innerHTML = ''; // Remove the old canvas

    scene = new THREE.Scene();
    scene.background = new THREE.Color(0x111111);

    camera = new THREE.PerspectiveCamera(45, container.clientWidth / container.clientHeight, 0.1, 1000);
    camera.position.set(200, 150, 300);
    camera.lookAt(0, 100, 0);

    renderer = new THREE.WebGLRenderer({ antialias: true });
    renderer.setSize(container.clientWidth, container.clientHeight);
    container.appendChild(renderer.domElement);

    // Lighting
    const ambientLight = new THREE.AmbientLight(0x404040, 2);
    scene.add(ambientLight);
    const dirLight = new THREE.DirectionalLight(0xffffff, 1);
    dirLight.position.set(100, 200, 50);
    scene.add(dirLight);

    // Grid
    const gridHelper = new THREE.GridHelper(400, 20, 0x00f0ff, 0x444444);
    scene.add(gridHelper);

    // Geometries
    const geometries = [
        new THREE.BoxGeometry(40, 20, 40), // J0: Base
        new THREE.BoxGeometry(20, 80, 20), // J1: Shoulder
        new THREE.BoxGeometry(15, 70, 15), // J2: Elbow
        new THREE.BoxGeometry(10, 50, 10), // J3: Wrist Pitch
        new THREE.BoxGeometry(8, 30, 8),   // J4: Wrist Roll
        null // J5: Gripper (Custom constructed below)
    ];

    const materials = [
        new THREE.MeshPhongMaterial({ color: 0x444444, wireframe: true }),
        new THREE.MeshPhongMaterial({ color: 0x00f0ff, wireframe: true }),
        new THREE.MeshPhongMaterial({ color: 0x00f0ff, wireframe: true }),
        new THREE.MeshPhongMaterial({ color: 0x00f0ff, wireframe: true }),
        new THREE.MeshPhongMaterial({ color: 0x00f0ff, wireframe: true }),
        new THREE.MeshPhongMaterial({ color: 0xff4757, wireframe: true })
    ];

    const offsets = [
        new THREE.Vector3(0, 0, 0),      // Base on floor
        new THREE.Vector3(0, 20, 0),     // Shoulder above base
        new THREE.Vector3(0, 80, 0),     // Elbow above shoulder
        new THREE.Vector3(0, 70, 0),     // Wrist P above elbow
        new THREE.Vector3(0, 50, 0),     // Wrist R above Wrist P
        new THREE.Vector3(0, 30, 0)      // EE above Wrist R
    ];

    let currentParent = scene;

    for (let i = 0; i < 6; i++) {
        const jointPivot = new THREE.Group();
        jointPivot.position.copy(offsets[i]);

        if (i === 5) { // Gripper Claw
            const clawGroup = new THREE.Group();
            const baseMaterial = materials[i];

            const gripperBase = new THREE.Mesh(new THREE.BoxGeometry(24, 6, 12), baseMaterial);
            gripperBase.position.y = 3;
            clawGroup.add(gripperBase);

            const leftFinger = new THREE.Mesh(new THREE.BoxGeometry(4, 25, 8), baseMaterial);
            leftFinger.position.set(-8, 15, 0);
            clawGroup.add(leftFinger);

            const rightFinger = new THREE.Mesh(new THREE.BoxGeometry(4, 25, 8), baseMaterial);
            rightFinger.position.set(8, 15, 0);
            clawGroup.add(rightFinger);

            jointPivot.add(clawGroup);

            jointPivot.userData.leftFinger = leftFinger;
            jointPivot.userData.rightFinger = rightFinger;
        } else {
            const mesh = new THREE.Mesh(geometries[i], materials[i]);
            if (i === 0) {
                mesh.position.y = 10;
            } else {
                mesh.position.y = geometries[i].parameters.height / 2;
            }
            jointPivot.add(mesh);
        }

        currentParent.add(jointPivot);
        armJoints.push(jointPivot);
        currentParent = jointPivot;
    }

    animate();
}

function updateArmAngles() {
    if (armJoints.length === 0) return;
    const toRad = (deg) => (deg - 90) * (Math.PI / 180);

    armJoints[0].rotation.y = toRad(state.angles[0]);
    armJoints[1].rotation.z = toRad(state.angles[1]);
    armJoints[2].rotation.z = toRad(state.angles[2]);
    armJoints[3].rotation.z = toRad(state.angles[3]);
    armJoints[4].rotation.y = toRad(state.angles[4]);

    const gripperPivot = armJoints[5];
    if (gripperPivot.userData.leftFinger) {
        const jd = jointDefs[5];
        const clampedAngle = Math.max(jd.min, Math.min(jd.max, state.angles[5]));
        const range = jd.max - jd.min;
        const openFactor = range === 0 ? 0 : (clampedAngle - jd.min) / range;

        gripperPivot.userData.leftFinger.position.x = -8 - (openFactor * 10);
        gripperPivot.userData.rightFinger.position.x = 8 + (openFactor * 10);
    }
}

function animate() {
    requestAnimationFrame(animate);
    updateArmAngles();
    renderer.render(scene, camera);
}


// ---------------------------------------------------------
// Sequence Recorder
// ---------------------------------------------------------
document.getElementById('btnAddFrame').addEventListener('click', () => {
    const frame = [...state.angles];
    state.sequence.push(frame);
    updateSequenceList();
    logConsole(`Frame added to sequence`, 'sys');
});

function updateSequenceList() {
    sequenceList.innerHTML = '';
    state.sequence.forEach((frame, idx) => {
        const div = document.createElement('div');
        div.innerText = `Frame ${idx + 1}: A ${frame.join(' ')}`;
        sequenceList.appendChild(div);
    });
}

// Sequence Playback Logic
let isPlaying = false;

document.getElementById('btnPlay').addEventListener('click', async () => {
    if (isPlaying || state.sequence.length === 0) {
        logConsole('No frames to play or already playing.', 'sys');
        return;
    }

    isPlaying = true;
    logConsole('Starting sequence playback...', 'sys');

    for (let i = 0; i < state.sequence.length; i++) {
        if (!isPlaying) break; // Stop button pressed
        const frame = state.sequence[i];

        logConsole(`Playing Frame ${i + 1}: A ${frame.join(' ')}`, 'sys');

        // Update State and UI Sliders
        state.angles = [...frame];
        document.querySelectorAll('.motor-slider').forEach((slider, idx) => {
            slider.value = state.angles[idx];
            document.getElementById(`val-m${idx}`).innerText = `${state.angles[idx]}°`;
        });

        // Send actual command
        sendCommand(`A ${frame.join(' ')}`);

        // Wait for hardware/mocked motion (simulate 1 second delay)
        await new Promise(resolve => setTimeout(resolve, 1000));
    }

    isPlaying = false;
    logConsole('Sequence playback complete.', 'sys');
});

document.getElementById('btnStopSeq').addEventListener('click', () => {
    if (isPlaying) {
        isPlaying = false;
        logConsole('Sequence playback stopped by user.', 'sys');
    }
});


// ---------------------------------------------------------
// Top Level Handlers & Init
// ---------------------------------------------------------
initSliders();
initQuickActions();
init3D();
logConsole('System Initialized. 3D Engine Ready.', 'sys');

// Buttons
document.getElementById('btnHome').addEventListener('click', () => {
    state.angles = jointDefs.map(jd => jd.home);
    document.querySelectorAll('.motor-slider').forEach((slider, i) => {
        slider.value = state.angles[i];
        document.getElementById(`val-m${i}`).innerText = `${state.angles[i]}°`;
    });
    sendCommand('H');
});

document.getElementById('btnStop').addEventListener('click', () => {
    sendCommand('X');
    logConsole('EMERGENCY STOP', 'sys');
});

document.getElementById('btnSync').addEventListener('click', () => {
    sendCommand('T');
});

// Zoom Controls
const btnZoomIn = document.getElementById('btnZoomIn');
const btnZoomOut = document.getElementById('btnZoomOut');
if (btnZoomIn) {
    btnZoomIn.addEventListener('click', () => {
        camera.fov = Math.max(10, camera.fov - 10);
        camera.updateProjectionMatrix();
    });
}
if (btnZoomOut) {
    btnZoomOut.addEventListener('click', () => {
        camera.fov = Math.min(100, camera.fov + 10);
        camera.updateProjectionMatrix();
    });
}

// Handle window resize
window.addEventListener('resize', () => {
    if (camera && renderer) {
        camera.aspect = container.clientWidth / container.clientHeight;
        camera.updateProjectionMatrix();
        renderer.setSize(container.clientWidth, container.clientHeight);
    }
});

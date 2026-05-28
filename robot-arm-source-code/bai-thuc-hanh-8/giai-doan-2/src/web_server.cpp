// =============================================================
//  web_server.cpp  –  WiFi connection & REST API / Web Server
// =============================================================
#include "web_server.h"
#include "config.h"
#include <WiFi.h>
#include <WebServer.h>

static WebServer server(CFG_WEB_PORT);
static bool wifiConnected = false;

// ── Gorgeous Cyber-Dark Dashboard Embedded in Flash ──────────
static const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Robotic Arm Dashboard</title>
    <link href="https://fonts.googleapis.com/css2?family=Outfit:wght@300;400;600;800&family=JetBrains+Mono:wght@400;700&display=swap" rel="stylesheet">
    <style>
        :root {
            --bg-base: #080b11;
            --bg-grid: radial-gradient(circle at 50% 50%, #161d30 0%, #080b11 100%);
            --bg-card: rgba(18, 25, 41, 0.65);
            --bg-term: #05070a;
            --accent-glow: linear-gradient(135deg, #6366f1, #a855f7);
            --accent-color: #6366f1;
            --border-glow: rgba(99, 102, 241, 0.2);
            --border: rgba(255, 255, 255, 0.08);
            --text-main: #f1f5f9;
            --text-muted: #94a3b8;
            --success: #10b981;
            --danger: #ef4444;
            --warning: #f59e0b;
        }

        * {
            box-sizing: border-box;
            margin: 0;
            padding: 0;
        }

        body {
            font-family: 'Outfit', sans-serif;
            background-color: var(--bg-base);
            background-image: var(--bg-grid);
            color: var(--text-main);
            min-height: 100vh;
            padding: 20px;
            display: flex;
            flex-direction: column;
            align-items: center;
        }

        .container {
            width: 100%;
            max-width: 1200px;
            display: flex;
            flex-direction: column;
            gap: 20px;
        }

        /* ── Header Styling ── */
        header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            padding: 20px;
            background: var(--bg-card);
            backdrop-filter: blur(12px);
            border: 1px solid var(--border);
            border-radius: 16px;
            box-shadow: 0 8px 32px 0 rgba(0, 0, 0, 0.37);
        }

        .logo-area {
            display: flex;
            align-items: center;
            gap: 15px;
        }

        .logo-icon {
            width: 40px;
            height: 40px;
            background: var(--accent-glow);
            border-radius: 10px;
            display: flex;
            align-items: center;
            justify-content: center;
            font-weight: 800;
            font-size: 20px;
            color: white;
            box-shadow: 0 0 15px var(--accent-color);
        }

        .title-area h1 {
            font-size: 22px;
            font-weight: 800;
            letter-spacing: 1px;
            background: var(--accent-glow);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
        }

        .title-area p {
            font-size: 12px;
            color: var(--text-muted);
        }

        .status-pill {
            display: flex;
            align-items: center;
            gap: 8px;
            padding: 8px 16px;
            background: rgba(16, 185, 129, 0.1);
            border: 1px solid rgba(16, 185, 129, 0.3);
            border-radius: 30px;
            font-size: 13px;
            color: var(--success);
            font-weight: 600;
        }

        .status-pill.offline {
            background: rgba(239, 68, 68, 0.1);
            border: 1px solid rgba(239, 68, 68, 0.3);
            color: var(--danger);
        }

        .status-dot {
            width: 8px;
            height: 8px;
            border-radius: 50%;
            background-color: var(--success);
            box-shadow: 0 0 8px var(--success);
            animation: pulse 1.5s infinite;
        }

        .status-pill.offline .status-dot {
            background-color: var(--danger);
            box-shadow: 0 0 8px var(--danger);
        }

        @keyframes pulse {
            0% { transform: scale(0.9); opacity: 0.6; }
            50% { transform: scale(1.1); opacity: 1; }
            100% { transform: scale(0.9); opacity: 0.6; }
        }

        /* ── Main Layout Grid ── */
        .dashboard-grid {
            display: grid;
            grid-template-columns: 2fr 1fr;
            gap: 20px;
        }

        @media (max-width: 900px) {
            .dashboard-grid {
                grid-template-columns: 1fr;
            }
        }

        .card {
            background: var(--bg-card);
            backdrop-filter: blur(12px);
            border: 1px solid var(--border);
            border-radius: 16px;
            padding: 24px;
            box-shadow: 0 8px 32px 0 rgba(0, 0, 0, 0.25);
            transition: border-color 0.3s ease;
        }

        .card:hover {
            border-color: var(--border-glow);
        }

        .card-title {
            font-size: 18px;
            font-weight: 600;
            margin-bottom: 20px;
            display: flex;
            justify-content: space-between;
            align-items: center;
            border-bottom: 1px solid var(--border);
            padding-bottom: 10px;
            color: var(--text-main);
        }

        /* ── Joint Controls ── */
        .joints-container {
            display: flex;
            flex-direction: column;
            gap: 20px;
        }

        .joint-row {
            display: grid;
            grid-template-columns: 140px 1fr 90px;
            align-items: center;
            gap: 15px;
            background: rgba(255, 255, 255, 0.02);
            padding: 12px 16px;
            border-radius: 12px;
            border: 1px solid rgba(255, 255, 255, 0.03);
            transition: all 0.2s ease;
        }

        .joint-row:hover {
            background: rgba(255, 255, 255, 0.04);
            border-color: rgba(99, 102, 241, 0.1);
        }

        .joint-info {
            display: flex;
            flex-direction: column;
            gap: 4px;
        }

        .joint-name {
            font-weight: 600;
            font-size: 15px;
        }

        .joint-tag {
            font-size: 11px;
            color: var(--text-muted);
            font-family: 'JetBrains Mono', monospace;
        }

        .slider-container {
            display: flex;
            align-items: center;
            gap: 15px;
            position: relative;
        }

        .joint-slider {
            flex-grow: 1;
            height: 8px;
            -webkit-appearance: none;
            background: #1e293b;
            border-radius: 4px;
            outline: none;
            cursor: pointer;
        }

        .joint-slider::-webkit-slider-thumb {
            -webkit-appearance: none;
            width: 20px;
            height: 20px;
            border-radius: 50%;
            background: var(--accent-glow);
            box-shadow: 0 0 10px var(--accent-color);
            transition: transform 0.1s;
        }

        .joint-slider::-webkit-slider-thumb:hover {
            transform: scale(1.2);
        }

        .slider-limits {
            font-size: 11px;
            color: var(--text-muted);
            min-width: 25px;
            text-align: center;
        }

        .angle-badge {
            font-family: 'JetBrains Mono', monospace;
            background: rgba(99, 102, 241, 0.15);
            border: 1px solid var(--border-glow);
            color: #a5b4fc;
            padding: 6px 10px;
            border-radius: 8px;
            text-align: center;
            font-weight: 700;
            font-size: 14px;
        }

        /* ── Side Panels & Presets ── */
        .side-panels {
            display: flex;
            flex-direction: column;
            gap: 20px;
        }

        .button-grid {
            display: grid;
            grid-template-columns: 1fr;
            gap: 12px;
        }

        .btn {
            width: 100%;
            padding: 12px 20px;
            border: none;
            border-radius: 10px;
            font-family: 'Outfit', sans-serif;
            font-size: 14px;
            font-weight: 600;
            cursor: pointer;
            transition: all 0.2s ease;
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 10px;
        }

        .btn-primary {
            background: var(--accent-glow);
            color: white;
            box-shadow: 0 4px 15px rgba(99, 102, 241, 0.25);
        }

        .btn-primary:hover {
            transform: translateY(-2px);
            box-shadow: 0 6px 20px rgba(99, 102, 241, 0.35);
        }

        .btn-danger {
            background: linear-gradient(135deg, #ef4444, #b91c1c);
            color: white;
            box-shadow: 0 4px 15px rgba(239, 68, 68, 0.25);
            font-size: 15px;
            font-weight: 800;
            letter-spacing: 0.5px;
        }

        .btn-danger:hover {
            transform: translateY(-2px);
            box-shadow: 0 6px 20px rgba(239, 68, 68, 0.45);
            filter: brightness(1.1);
        }

        .btn-secondary {
            background: #1e293b;
            color: var(--text-main);
            border: 1px solid var(--border);
        }

        .btn-secondary:hover {
            background: #334155;
            border-color: rgba(255, 255, 255, 0.15);
        }

        /* ── Speed Settings Panel ── */
        .speed-grid {
            display: grid;
            grid-template-columns: repeat(2, 1fr);
            gap: 10px;
        }

        .speed-item {
            background: rgba(255, 255, 255, 0.01);
            border: 1px solid var(--border);
            padding: 10px;
            border-radius: 10px;
            display: flex;
            flex-direction: column;
            gap: 6px;
        }

        .speed-item label {
            font-size: 12px;
            color: var(--text-muted);
        }

        .speed-input {
            width: 100%;
            background: #0f172a;
            border: 1px solid var(--border);
            color: white;
            padding: 6px;
            border-radius: 6px;
            font-family: 'JetBrains Mono', monospace;
            font-size: 13px;
        }

        /* ── Terminal Card ── */
        .terminal-container {
            display: flex;
            flex-direction: column;
            gap: 10px;
            height: 240px;
        }

        .terminal-log {
            flex-grow: 1;
            background: var(--bg-term);
            border: 1px solid var(--border);
            border-radius: 10px;
            padding: 12px;
            overflow-y: auto;
            font-family: 'JetBrains Mono', monospace;
            font-size: 12px;
            color: #38bdf8;
            display: flex;
            flex-direction: column;
            gap: 6px;
        }

        .term-line {
            line-height: 1.5;
            white-space: pre-wrap;
        }

        .term-in {
            color: #a7f3d0;
        }

        .term-out {
            color: #f8fafc;
        }

        .term-err {
            color: var(--danger);
        }

        .terminal-input-container {
            display: flex;
            gap: 10px;
        }

        .terminal-input {
            flex-grow: 1;
            background: var(--bg-term);
            border: 1px solid var(--border);
            color: white;
            padding: 10px 14px;
            border-radius: 8px;
            font-family: 'JetBrains Mono', monospace;
            font-size: 13px;
            outline: none;
        }

        .terminal-input:focus {
            border-color: var(--accent-color);
            box-shadow: 0 0 5px rgba(99, 102, 241, 0.3);
        }

        /* ── Sync Animation Overlay ── */
        .syncing-overlay {
            position: absolute;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background: rgba(8, 11, 17, 0.7);
            display: flex;
            justify-content: center;
            align-items: center;
            border-radius: 12px;
            opacity: 0;
            pointer-events: none;
            transition: opacity 0.3s ease;
            z-index: 10;
        }

        .syncing-overlay.active {
            opacity: 1;
            pointer-events: auto;
        }

        .spinner {
            width: 25px;
            height: 25px;
            border: 3px solid rgba(255,255,255,0.1);
            border-top: 3px solid var(--accent-color);
            border-radius: 50%;
            animation: spin 0.8s linear infinite;
        }

        @keyframes spin {
            0% { transform: rotate(0deg); }
            100% { transform: rotate(360deg); }
        }
    </style>
</head>
<body>

<div class="container">
    <!-- ── Header ── -->
    <header>
        <div class="logo-area">
            <div class="logo-icon">🤖</div>
            <div class="title-area">
                <h1>ROBOTIC ARM CONTROLLER</h1>
                <p id="wifi-ip">Connecting to Web Server Telemetry...</p>
            </div>
        </div>
        <div id="status-pill" class="status-pill offline">
            <div class="status-dot"></div>
            <span id="status-text">OFFLINE</span>
        </div>
    </header>

    <!-- ── Main Dashboard ── -->
    <div class="dashboard-grid">
        <!-- Joint Control Card -->
        <div class="card" style="position: relative;">
            <div class="syncing-overlay" id="global-sync">
                <div style="display:flex; flex-direction:column; align-items:center; gap:10px;">
                    <div class="spinner"></div>
                    <span style="font-size:12px; color:var(--text-muted); font-weight:600;">WAITING FOR ARM MOTION...</span>
                </div>
            </div>
            
            <div class="card-title">
                <span>Joint Live Control</span>
                <span id="motion-indicator" style="font-size:12px; font-weight:normal; color:var(--text-muted);">Status: Idle</span>
            </div>
            
            <div class="joints-container">
                <!-- Joint rows will be populated dynamically -->
                <div id="joints-placeholder" style="text-align:center; padding:40px; color:var(--text-muted);">
                    Initializing joint nodes...
                </div>
            </div>
        </div>

        <!-- Right Side Panel -->
        <div class="side-panels">
            <!-- Action Presets -->
            <div class="card">
                <div class="card-title">Operations</div>
                <div class="button-grid">
                    <button class="btn btn-danger" onclick="stopAll()">🚨 EMERGENCY STOP</button>
                    <button class="btn btn-secondary" onclick="homeAll()">🏠 Home All Servos</button>
                    <button class="btn btn-secondary" onclick="waitForIdle()">⏳ Wait for Motion Complete</button>
                </div>
            </div>

            <!-- Joint Speed Settings -->
            <div class="card">
                <div class="card-title">Joint Speed (1-20 deg/tick)</div>
                <div class="speed-grid" id="speed-container">
                    <!-- Speeds will be populated here -->
                </div>
            </div>
        </div>
    </div>

    <!-- ── Full-Width Command Terminal ── -->
    <div class="card">
        <div class="card-title">Raw Command Terminal Bridge</div>
        <div class="terminal-container">
            <div class="terminal-log" id="term-log">
                <div class="term-line term-out">ESP32 Terminal initialized. Type commands like 'T', 'H', or 'M 0 90' and hit Enter.</div>
            </div>
            <div class="terminal-input-container">
                <input type="text" class="terminal-input" id="term-input" placeholder="Type raw command (e.g. M 0 90)" autocomplete="off">
                <button class="btn btn-primary" style="width: 100px; padding: 0 20px;" onclick="sendTerminalCmd()">Send</button>
            </div>
        </div>
    </div>
</div>

<script>
    const API_URL = ""; // Relative paths
    let servos = [];
    let isDragging = {};
    let pollInterval = null;

    // Fetch initial joint info
    async function initDashboard() {
        try {
            const res = await fetch(API_URL + "/api/info");
            const data = await res.json();
            if (data.status === "success") {
                const container = document.querySelector(".joints-container");
                container.innerHTML = "";
                const speedContainer = document.getElementById("speed-container");
                speedContainer.innerHTML = "";

                servos = data.servos;

                servos.forEach(servo => {
                    isDragging[servo.id] = false;

                    // Create Slider Row
                    const row = document.createElement("div");
                    row.className = "joint-row";
                    row.id = `joint-row-${servo.id}`;
                    row.innerHTML = `
                        <div class="joint-info">
                            <span class="joint-name">${servo.name}</span>
                            <span class="joint-tag">Joint ${servo.id} (ch ${servo.id})</span>
                        </div>
                        <div class="slider-container">
                            <span class="slider-limits">${servo.min}°</span>
                            <input type="range" class="joint-slider" id="slider-${servo.id}" 
                                   min="${servo.min}" max="${servo.max}" value="${servo.home}"
                                   oninput="updateAngleBadge(${servo.id}, this.value)"
                                   onchange="sendMoveCommand(${servo.id}, this.value)">
                            <span class="slider-limits">${servo.max}°</span>
                        </div>
                        <div class="angle-badge" id="badge-${servo.id}">${servo.home}°</div>
                    `;
                    container.appendChild(row);

                    // Add listeners to prevent auto-polling updates during drags
                    const slider = row.querySelector(".joint-slider");
                    slider.addEventListener("mousedown", () => { isDragging[servo.id] = true; });
                    slider.addEventListener("touchstart", () => { isDragging[servo.id] = true; });
                    slider.addEventListener("mouseup", () => { isDragging[servo.id] = false; });
                    slider.addEventListener("touchend", () => { isDragging[servo.id] = false; });

                    // Create Speed Item
                    const speedItem = document.createElement("div");
                    speedItem.className = "speed-item";
                    speedItem.innerHTML = `
                        <label>${servo.name} (J${servo.id})</label>
                        <input type="number" class="speed-input" id="speed-${servo.id}" 
                               min="1" max="20" value="${servo.defaultSpeed}" 
                               onchange="sendSpeedCommand(${servo.id}, this.value)">
                    `;
                    speedContainer.appendChild(speedItem);
                });

                // Update UI header
                document.getElementById("wifi-ip").innerText = "ESP32 Host IP: " + window.location.host;
                const statusPill = document.getElementById("status-pill");
                statusPill.className = "status-pill";
                document.getElementById("status-text").innerText = "CONNECTED";

                // Start active polling
                pollInterval = setInterval(pollStatus, 250);
                pollStatus();
            }
        } catch (err) {
            console.error("Failed to connect to ESP32 API:", err);
            document.getElementById("wifi-ip").innerText = "ESP32 Offline / Unavailable";
            const statusPill = document.getElementById("status-pill");
            statusPill.className = "status-pill offline";
            document.getElementById("status-text").innerText = "OFFLINE";
        }
    }

    // Poll status in loop
    async function pollStatus() {
        try {
            const res = await fetch(API_URL + "/api/status");
            const data = await res.json();
            if (data.status === "success") {
                // Update header status
                const statusPill = document.getElementById("status-pill");
                statusPill.className = "status-pill";
                document.getElementById("status-text").innerText = "CONNECTED";

                // Update global motion text
                const motionIndicator = document.getElementById("motion-indicator");
                if (data.moving) {
                    motionIndicator.innerHTML = '<span style="color:var(--warning); animation: pulse 1s infinite;">Status: Moving</span>';
                } else {
                    motionIndicator.innerText = "Status: Idle";
                }

                // Update sliders
                data.servos.forEach(servo => {
                    const badge = document.getElementById(`badge-${servo.id}`);
                    if (badge) {
                        badge.innerText = `${servo.angle}°`;
                        if (servo.moving) {
                            badge.style.borderColor = "var(--warning)";
                            badge.style.background = "rgba(245, 158, 11, 0.15)";
                            badge.style.color = "#fef08a";
                        } else {
                            badge.style.borderColor = "var(--border-glow)";
                            badge.style.background = "rgba(99, 102, 241, 0.15)";
                            badge.style.color = "#a5b4fc";
                        }
                    }

                    // Update slider value only if not actively dragging
                    if (!isDragging[servo.id]) {
                        const slider = document.getElementById(`slider-${servo.id}`);
                        if (slider) slider.value = servo.target;
                    }

                    // Update speed value
                    const speedInput = document.getElementById(`speed-${servo.id}`);
                    if (speedInput && document.activeElement !== speedInput) {
                        speedInput.value = servo.speed;
                    }
                });
            }
        } catch (err) {
            console.error("Polling error:", err);
        }
    }

    // Slider UI updates (local/immediate visual feedback)
    function updateAngleBadge(id, val) {
        const badge = document.getElementById(`badge-${id}`);
        if (badge) {
            badge.innerText = val + "°";
            badge.style.color = "white";
        }
    }

    // POST calls for movements
    async function sendMoveCommand(id, angle) {
        try {
            const formData = new FormData();
            formData.append("id", id);
            formData.append("angle", angle);
            
            const res = await fetch(API_URL + "/api/move", {
                method: "POST",
                body: formData
            });
            const data = await res.json();
            if (data.status !== "success") {
                appendTerminalLine(`ERR: Failed to move Joint ${id}: ${data.msg}`, "err");
            }
        } catch (err) {
            appendTerminalLine(`Network Error trying to move Joint ${id}`, "err");
        }
    }

    // POST calls for joint speeds
    async function sendSpeedCommand(id, speed) {
        if (speed < 1 || speed > 20) return;
        try {
            const formData = new FormData();
            formData.append("id", id);
            formData.append("speed", speed);

            const res = await fetch(API_URL + "/api/speed", {
                method: "POST",
                body: formData
            });
            const data = await res.json();
            if (data.status === "success") {
                appendTerminalLine(`Speed configured: Joint ${id} = ${speed} deg/tick`, "out");
            } else {
                appendTerminalLine(`ERR: Setting speed failed: ${data.msg}`, "err");
            }
        } catch (err) {
            appendTerminalLine(`Network Error saving speed for Joint ${id}`, "err");
        }
    }

    // Operations
    async function stopAll() {
        try {
            appendTerminalLine("EMERGENCY STOP SENT", "err");
            const res = await fetch(API_URL + "/api/stop", { method: "POST" });
            const data = await res.json();
            pollStatus();
        } catch (err) {
            console.error(err);
        }
    }

    async function homeAll() {
        try {
            appendTerminalLine("HOMING ALL JOINTS", "out");
            const res = await fetch(API_URL + "/api/home", { method: "POST" });
            const data = await res.json();
            pollStatus();
        } catch (err) {
            console.error(err);
        }
    }

    async function waitForIdle() {
        const overlay = document.getElementById("global-sync");
        overlay.classList.add("active");
        appendTerminalLine("Waiting for all motion complete...", "out");
        try {
            const res = await fetch(API_URL + "/api/wait", { method: "POST" });
            const data = await res.json();
            if (data.status === "success") {
                appendTerminalLine("Arm is idle. Sync complete.", "out");
            }
        } catch (err) {
            appendTerminalLine("Wait timed out or disconnected", "err");
        } finally {
            overlay.classList.remove("active");
            pollStatus();
        }
    }

    // Terminal logic
    function appendTerminalLine(text, type = "out") {
        const log = document.getElementById("term-log");
        const line = document.createElement("div");
        line.className = `term-line term-${type}`;
        line.innerText = text;
        log.appendChild(line);
        log.scrollTop = log.scrollHeight;
    }

    async function sendTerminalCmd() {
        const input = document.getElementById("term-input");
        const cmd = input.value.trim();
        if (!cmd) return;

        appendTerminalLine(`> ${cmd}`, "in");
        input.value = "";

        try {
            const formData = new FormData();
            formData.append("cmd", cmd);

            const res = await fetch(API_URL + "/api/command", {
                method: "POST",
                body: formData
            });
            const data = await res.json();
            if (data.status === "success") {
                // Formatting clean output text response
                const formattedResp = data.response.replace(/\\n/g, "\n");
                appendTerminalLine(formattedResp, "out");
            } else {
                appendTerminalLine(`Error executing: ${data.msg}`, "err");
            }
        } catch (err) {
            appendTerminalLine("Terminal link execution failed", "err");
        }
    }

    // Bind Enter key in terminal
    document.getElementById("term-input").addEventListener("keypress", function(e) {
        if (e.key === "Enter") {
            sendTerminalCmd();
        }
    });

    // Run setup
    window.onload = initDashboard;
</script>
</body>
</html>
)rawliteral";

// ── Web Server Initialization & Routes Implementation ────────
void webServerInit(CmdParser& parser, ServoCtrl& servos) {
    // ── Connect to lab WiFi AP ──
    Serial.println();
    Serial.print(F("Connecting to WiFi AP '"));
    Serial.print(CFG_WIFI_SSID);
    Serial.println(F("'..."));

    WiFi.mode(WIFI_STA);
    WiFi.begin(CFG_WIFI_SSID, CFG_WIFI_PASSWORD);

    uint32_t startMs = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - startMs < CFG_WIFI_TIMEOUT_MS)) {
        delay(500);
        Serial.print(F("."));
    }

    if (WiFi.status() == WL_CONNECTED) {
        wifiConnected = true;
        Serial.println();
        Serial.print(F("WiFi Connected! ESP32 IP Address: "));
        Serial.println(WiFi.localIP());
    } else {
        Serial.println();
        Serial.println(F("WiFi Connection Failed (Timeout). Continuing in Serial-only mode."));
        return; // Web server will not run if connection failed
    }

    // ── HTTP Router Handlers ──

    // 1. Root: Serves dashboard
    server.on("/", HTTP_GET, []() {
        server.send_P(200, "text/html", INDEX_HTML);
    });

    // 2. Status: Live status telemetry JSON
    server.on("/api/status", HTTP_GET, [&servos]() {
        String json = "{\n  \"status\": \"success\",\n  \"moving\": " + String(servos.anyMoving() ? "true" : "false") + ",\n  \"servos\": [\n";
        for (uint8_t i = 0; i < CFG_NUM_SERVOS; i++) {
            json += "    {\n";
            json += "      \"id\": " + String(i) + ",\n";
            json += "      \"name\": \"" + String(i == 0 ? "Base" : 
                                           i == 1 ? "Shoulder" :
                                           i == 2 ? "Elbow" :
                                           i == 3 ? "Wrist Pitch" :
                                           i == 4 ? "Wrist Roll" : "Gripper") + "\",\n";
            json += "      \"angle\": " + String(servos.getAngle(i)) + ",\n";
            json += "      \"target\": " + String(servos.getTarget(i)) + ",\n";
            json += "      \"speed\": " + String(servos.getSpeed(i)) + ",\n";
            json += "      \"moving\": " + String(servos.isMoving(i) ? "true" : "false") + "\n";
            json += "    }" + String(i < CFG_NUM_SERVOS - 1 ? "," : "") + "\n";
        }
        json += "  ]\n}";
        server.send(200, "application/json", json);
    });

    // 3. Info: Servo metadata configs JSON
    server.on("/api/info", HTTP_GET, []() {
        String json = "{\n  \"status\": \"success\",\n  \"servos\": [\n";
        for (uint8_t i = 0; i < CFG_NUM_SERVOS; i++) {
            const ServoDef& d = SERVO_TABLE[i];
            json += "    {\n";
            json += "      \"id\": " + String(i) + ",\n";
            json += "      \"name\": \"" + String(i == 0 ? "Base" : 
                                           i == 1 ? "Shoulder" :
                                           i == 2 ? "Elbow" :
                                           i == 3 ? "Wrist Pitch" :
                                           i == 4 ? "Wrist Roll" : "Gripper") + "\",\n";
            json += "      \"min\": " + String(d.minAng) + ",\n";
            json += "      \"max\": " + String(d.maxAng) + ",\n";
            json += "      \"home\": " + String(d.homeAng) + ",\n";
            json += "      \"defaultSpeed\": " + String(d.defSpeed) + "\n";
            json += "    }" + String(i < CFG_NUM_SERVOS - 1 ? "," : "") + "\n";
        }
        json += "  ]\n}";
        server.send(200, "application/json", json);
    });

    // 4. Move: Angle target set
    server.on("/api/move", HTTP_POST, [&servos]() {
        if (!server.hasArg("id") || !server.hasArg("angle")) {
            server.send(400, "application/json", "{\"status\":\"error\",\"msg\":\"ERR:ARGS\"}");
            return;
        }
        uint8_t id = server.arg("id").toInt();
        uint8_t angle = server.arg("angle").toInt();
        if (servos.moveTo(id, angle)) {
            server.send(200, "application/json", "{\"status\":\"success\",\"msg\":\"OK\"}");
        } else {
            server.send(400, "application/json", "{\"status\":\"error\",\"msg\":\"ERR:ID\"}");
        }
    });

    // 5. Move All: Set multiple joints
    server.on("/api/move_all", HTTP_POST, [&servos]() {
        if (server.hasArg("angles")) {
            String anglesStr = server.arg("angles");
            char buf[64];
            anglesStr.toCharArray(buf, sizeof(buf));
            char* p = buf;
            for (uint8_t i = 0; i < CFG_NUM_SERVOS; i++) {
                while (*p == ' ' || *p == ',') p++;
                if (*p == '\0') {
                    server.send(400, "application/json", "{\"status\":\"error\",\"msg\":\"ERR:ARGS\"}");
                    return;
                }
                uint8_t angle = (uint8_t)strtol(p, &p, 10);
                servos.moveTo(i, angle);
            }
            server.send(200, "application/json", "{\"status\":\"success\",\"msg\":\"OK\"}");
            return;
        }

        bool foundAny = false;
        for (uint8_t i = 0; i < CFG_NUM_SERVOS; i++) {
            String paramName = "a" + String(i);
            if (server.hasArg(paramName)) {
                uint8_t angle = server.arg(paramName).toInt();
                servos.moveTo(i, angle);
                foundAny = true;
            }
        }
        if (foundAny) {
            server.send(200, "application/json", "{\"status\":\"success\",\"msg\":\"OK\"}");
        } else {
            server.send(400, "application/json", "{\"status\":\"error\",\"msg\":\"ERR:ARGS\"}");
        }
    });

    // 6. Speed: Adjust joint speeds
    server.on("/api/speed", HTTP_POST, [&servos]() {
        if (!server.hasArg("id") || !server.hasArg("speed")) {
            server.send(400, "application/json", "{\"status\":\"error\",\"msg\":\"ERR:ARGS\"}");
            return;
        }
        uint8_t id = server.arg("id").toInt();
        uint8_t speed = server.arg("speed").toInt();
        if (servos.setSpeed(id, speed)) {
            server.send(200, "application/json", "{\"status\":\"success\",\"msg\":\"OK\"}");
        } else {
            server.send(400, "application/json", "{\"status\":\"error\",\"msg\":\"ERR:ID\"}");
        }
    });

    // 7. Home: Home one or all
    server.on("/api/home", HTTP_POST, [&servos]() {
        if (server.hasArg("id")) {
            uint8_t id = server.arg("id").toInt();
            servos.homeOne(id);
        } else {
            servos.home();
        }
        server.send(200, "application/json", "{\"status\":\"success\",\"msg\":\"OK\"}");
    });

    // 8. Stop: Halt all joints immediately
    server.on("/api/stop", HTTP_POST, [&servos]() {
        servos.stopAll();
        server.send(200, "application/json", "{\"status\":\"success\",\"msg\":\"OK\"}");
    });

    // 9. Wait: Block response until idle
    server.on("/api/wait", HTTP_POST, [&servos]() {
        uint32_t lastTick = millis();
        while (servos.anyMoving()) {
            uint32_t now = millis();
            if (now - lastTick >= CFG_STEP_MS) {
                lastTick = now;
                servos.update();
            }
            delay(5); // yield to prevent watchdog trigger and support WiFi processing
        }
        server.send(200, "application/json", "{\"status\":\"success\",\"msg\":\"DONE\"}");
    });

    // 10. Command: Raw serial-to-http bridge channel
    server.on("/api/command", HTTP_POST, [&parser]() {
        if (!server.hasArg("cmd")) {
            server.send(400, "application/json", "{\"status\":\"error\",\"msg\":\"ERR:ARGS\"}");
            return;
        }
        String cmd = server.arg("cmd");
        String response = parser.execute(cmd.c_str());

        // Escape JSON reserved characters inside response
        response.replace("\"", "\\\"");
        response.replace("\r", "");
        response.replace("\n", "\\n");

        String json = "{\n  \"status\": \"success\",\n  \"response\": \"" + response + "\"\n}";
        server.send(200, "application/json", json);
    });

    // Start server listening
    server.begin();
    Serial.println(F("Embedded Web Server successfully active!"));
}

void webServerHandle() {
    if (wifiConnected) {
        server.handleClient();
    }
}

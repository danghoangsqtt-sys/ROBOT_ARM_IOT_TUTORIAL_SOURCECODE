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
    angles: jointDefs.map(jd => jd.home)
};

const slidersContainer = document.getElementById('sliders');
const consoleOutput = document.getElementById('consoleOutput');

function logConsole(message, type = 'sys') {
    const div = document.createElement('div');
    div.className = `console-msg-${type}`;
    const timestamp = new Date().toLocaleTimeString();
    div.innerText = `[${timestamp}] ${message}`;
    consoleOutput.appendChild(div);
    consoleOutput.scrollTop = consoleOutput.scrollHeight;
}

// Web Serial API
let serialPort = null;
let serialWriter = null;
let reader = null;
let keepReading = true;

async function connectSerial() {
    try {
        const baudRate = parseInt(document.getElementById('baudRateSelect').value);
        serialPort = await navigator.serial.requestPort();
        await serialPort.open({ baudRate: baudRate });

        document.getElementById('connIndicator').className = 'indicator connected';
        document.getElementById('connectBtn').innerText = 'Disconnect';
        logConsole(`Connected to COM port at ${baudRate} baud.`, 'sys');

        serialWriter = serialPort.writable.getWriter();
        keepReading = true;
        readLoop();
    } catch (e) {
        logConsole('Serial connection failed or cancelled.', 'sys');
        console.error(e);
    }
}

async function disconnectSerial() {
    try {
        keepReading = false;
        if (reader) {
            await reader.cancel();
        }
        if (serialWriter) {
            serialWriter.releaseLock();
            serialWriter = null;
        }
        if (serialPort) {
            await serialPort.close();
            serialPort = null;
        }
        document.getElementById('connIndicator').className = 'indicator disconnected';
        document.getElementById('connectBtn').innerText = 'Connect via USB';
        logConsole('Disconnected from Serial Port.', 'sys');
    } catch (e) {
        logConsole('Error closing port: ' + e, 'sys');
    }
}

async function readLoop() {
    const decoder = new TextDecoderStream();
    const inputDone = serialPort.readable.pipeTo(decoder.writable);
    const inputStream = decoder.readable;
    reader = inputStream.getReader();

    try {
        while (keepReading) {
            const { value, done } = await reader.read();
            if (done) break;
            if (value) {
                // Try to parse status like STA:90,150,90,90,90,90
                const lines = value.split('\n');
                lines.forEach(line => {
                    const text = line.trim();
                    if(text.length > 0) {
                        logConsole(`RX: ${text}`, 'rx');
                        if (text.startsWith('STA:')) {
                            const parts = text.substring(4).split(',');
                            if (parts.length === 6) {
                                parts.forEach((p, idx) => {
                                    state.angles[idx] = parseInt(p);
                                    const slider = document.querySelector(`.motor-slider[data-id="${idx}"]`);
                                    if (slider) {
                                        slider.value = state.angles[idx];
                                        document.getElementById(`val-m${idx}`).innerText = `${state.angles[idx]}°`;
                                    }
                                });
                            }
                        }
                    }
                });
            }
        }
    } catch (error) {
        logConsole(`Read error: ${error}`, 'sys');
    } finally {
        reader.releaseLock();
    }
}

document.getElementById('connectBtn').addEventListener('click', async () => {
    if (serialPort) {
        await disconnectSerial();
    } else {
        await connectSerial();
    }
});

async function sendCommand(cmd) {
    logConsole(`TX: ${cmd}`, 'tx');
    if (serialPort && serialWriter) {
        try {
            const encoder = new TextEncoder();
            await serialWriter.write(encoder.encode(cmd + '\n'));
        } catch (e) {
            logConsole('Error writing to serial: ' + e, 'sys');
        }
    } else {
        logConsole('Port not connected. Open Console to view mock RX.', 'sys');
    }
}

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

initSliders();
logConsole('Web Serial API Ready. Please connect device via USB.', 'sys');

document.getElementById('btnHome').addEventListener('click', () => {
    sendCommand('H');
});

document.getElementById('btnStop').addEventListener('click', () => {
    sendCommand('X');
    logConsole('EMERGENCY STOP', 'sys');
});

document.getElementById('btnSync').addEventListener('click', () => {
    sendCommand('T');
});

const portSelect = document.getElementById('port');
const connectBtn = document.getElementById('connect');
const disconnectBtn = document.getElementById('disconnect');
const connectionStatus = document.getElementById('connection-status');
const sendHeartbeatBtn = document.getElementById('send-heartbeat');
const heartbeatDisplay = document.getElementById('heartbeat-display');
const logList = document.getElementById('log-list');
const ledOnCheckbox = document.getElementById('led-on');
const ledBrightness = document.getElementById('led-brightness');
const brightnessValue = document.getElementById('brightness-value');
const ledStatus = document.getElementById('led-status');

let ws = null;
let deviceConnected = false;
let wsConnected = false;

function getWsUrl() {
  const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
  return `${protocol}//${window.location.host}/ws`;
}

async function loadPorts() {
  const res = await fetch('/ports');
  const ports = await res.json();
  portSelect.innerHTML = '<option value="">Select port...</option>';
  for (const p of ports) {
    const opt = document.createElement('option');
    opt.value = p.device;
    opt.textContent = `${p.device} - ${p.description || 'Unknown'}`;
    portSelect.appendChild(opt);
  }
}

function setDeviceConnected(value) {
  deviceConnected = value;
  connectBtn.disabled = !wsConnected || value;
  disconnectBtn.disabled = !value;
  sendHeartbeatBtn.disabled = !value;
  ledOnCheckbox.disabled = !value;
  ledBrightness.disabled = !value;
  connectionStatus.textContent = value ? 'Connected' : 'Disconnected';
}

function connectWebSocket() {
  ws = new WebSocket(getWsUrl());
  ws.onopen = () => {
    wsConnected = true;
    connectBtn.disabled = false;
    loadPorts();
    setDeviceConnected(false);
  };
  ws.onmessage = (e) => {
    const msg = JSON.parse(e.data);
    handleMessage(msg);
  };
  ws.onclose = () => {
    wsConnected = false;
    setDeviceConnected(false);
    connectBtn.disabled = true;
    setTimeout(connectWebSocket, 2000);
  };
  ws.onerror = () => {};
}

function handleMessage(msg) {
  switch (msg.type) {
    case 'connected':
      setDeviceConnected(true);
      break;
    case 'disconnected':
      setDeviceConnected(false);
      break;
    case 'heartbeat':
      heartbeatDisplay.textContent = `Last: uptime=${msg.payload.uptime_ms}ms seq=${msg.payload.seq}`;
      break;
    case 'log_message':
      addLog(msg.payload);
      break;
    case 'generic_response':
      if (msg.payload.led_status) {
        ledOnCheckbox.checked = msg.payload.led_status.on;
        ledBrightness.value = msg.payload.led_status.brightness;
        brightnessValue.textContent = msg.payload.led_status.brightness;
        ledStatus.textContent = `Status: on=${msg.payload.led_status.on} brightness=${msg.payload.led_status.brightness}%`;
      }
      break;
    case 'error':
      connectionStatus.textContent = `Error: ${msg.payload.message}`;
      break;
  }
}

function addLog(entry) {
  const div = document.createElement('div');
  div.className = 'log-entry';
  const levelNames = ['DEBUG', 'INFO', 'WARN', 'ERROR'];
  const level = levelNames[entry.level] || '?';
  div.textContent = `[${level}] ${entry.tag}: ${entry.message}`;
  logList.insertBefore(div, logList.firstChild);
  while (logList.children.length > 100) {
    logList.removeChild(logList.lastChild);
  }
}

connectBtn.addEventListener('click', () => {
  const port = portSelect.value;
  if (!port || !ws || ws.readyState !== WebSocket.OPEN) return;
  ws.send(JSON.stringify({ type: 'connect', payload: { port } }));
});

disconnectBtn.addEventListener('click', () => {
  if (ws && ws.readyState === WebSocket.OPEN) {
    ws.send(JSON.stringify({ type: 'disconnect' }));
  }
});

sendHeartbeatBtn.addEventListener('click', () => {
  if (ws && ws.readyState === WebSocket.OPEN) {
    ws.send(JSON.stringify({ type: 'heartbeat' }));
  }
});

ledOnCheckbox.addEventListener('change', sendLedUpdate);
ledBrightness.addEventListener('input', () => {
  brightnessValue.textContent = ledBrightness.value;
  sendLedUpdate();
});

function sendLedUpdate() {
  if (ws && ws.readyState === WebSocket.OPEN && deviceConnected) {
    ws.send(JSON.stringify({
      type: 'set_led',
      payload: {
        on: ledOnCheckbox.checked,
        brightness: parseInt(ledBrightness.value, 10)
      }
    }));
  }
}

connectWebSocket();

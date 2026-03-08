const statusEl = document.getElementById('status');
const panels = {
  heartbeats: document.getElementById('panel-heartbeats'),
  utilCm4: document.getElementById('panel-util-cm4'),
  utilCm7: document.getElementById('panel-util-cm7'),
  peak: document.getElementById('panel-peak'),
  recent: document.getElementById('panel-recent')
};
const state = { cm4: null, cm7: null, utilMap: {}, utilTotals: null, peak: {}, recent: [] };
const MAX_RECENT = 80;
const CORE_PREFIX = { cm4: 'CM4|', cm7: 'CM7|' };
let eventSource = null;

function esc(s) {
  const d = document.createElement('div');
  d.textContent = s;
  return d.innerHTML;
}

function table(headers, rows, emptyCols) {
  if (!rows.length) return '<table><tbody><tr><td colspan="' + emptyCols + '" style="color:#555">—</td></tr></tbody></table>';
  let h = '<table><thead><tr>';
  headers.forEach(function (x) { h += '<th>' + esc(x) + '</th>'; });
  h += '</tr></thead><tbody>';
  rows.forEach(function (row) {
    h += '<tr' + (row.className ? ' class="' + row.className + '"' : '') + '>';
    row.cells.forEach(function (c) { h += '<td>' + (c.esc !== false ? esc(String(c.val)) : c.val) + '</td>'; });
    h += '</tr>';
  });
  return h + '</tbody></table>';
}

function addRecent(type, text, className) {
  state.recent.push({ type: type, text: text, className: className || '' });
  if (state.recent.length > MAX_RECENT) state.recent.shift();
  const rows = state.recent.map(function (r) {
    return { cells: [{ val: r.type }, { val: r.text }], className: r.className };
  });
  panels.recent.innerHTML = table(['Type', 'Message'], rows, 2);
  panels.recent.scrollTop = panels.recent.scrollHeight;
}

function renderHeartbeats() {
  const rows = [];
  if (state.cm4 != null) rows.push({ cells: [{ val: 'CM4' }, { val: state.cm4 }], className: 'heartbeat' });
  if (state.cm7 != null) rows.push({ cells: [{ val: 'CM7' }, { val: state.cm7 }], className: 'heartbeat' });
  panels.heartbeats.innerHTML = table(['Core', 'Seq'], rows, 2);
}

function renderUtil() {
  const keys4 = Object.keys(state.utilMap).filter(function (k) { return k.startsWith(CORE_PREFIX.cm4); }).sort();
  const rows4 = keys4.map(function (k) {
    return { cells: [{ val: k.slice(CORE_PREFIX.cm4.length) }, { val: state.utilMap[k] }], className: 'util' };
  });
  if (state.utilTotals) rows4.push({ cells: [{ val: 'Total' }, { val: state.utilTotals.CM4 + '%' }], className: 'util' });
  panels.utilCm4.innerHTML = table(['Task', 'Util %'], rows4, 2);

  const keys7 = Object.keys(state.utilMap).filter(function (k) { return k.startsWith(CORE_PREFIX.cm7); }).sort();
  const rows7 = keys7.map(function (k) {
    return { cells: [{ val: k.slice(CORE_PREFIX.cm7.length) }, { val: state.utilMap[k] }], className: 'util' };
  });
  if (state.utilTotals) rows7.push({ cells: [{ val: 'Total' }, { val: state.utilTotals.CM7 + '%' }], className: 'util' });
  panels.utilCm7.innerHTML = table(['Task', 'Util %'], rows7, 2);
}

function renderPeak() {
  const chs = Object.keys(state.peak).sort(function (a, b) { return Number(a) - Number(b); });
  const rows = chs.map(function (ch) { return { cells: [{ val: ch }, { val: state.peak[ch].toFixed(1) }] }; });
  panels.peak.innerHTML = table(['Channel', 'Peak (dB)'], rows, 2);
}

function onMessage(obj) {
  if (obj.type === 'HeartbeatCm4') {
    state.cm4 = obj.seq;
    renderHeartbeats();
    return;
  }
  if (obj.type === 'HeartbeatCm7') {
    state.cm7 = obj.seq;
    renderHeartbeats();
    return;
  }
  if (obj.type === 'TaskUtilCm4' || obj.type === 'TaskUtilCm7') {
    state.utilMap[obj.core + '|' + obj.task_name] = obj.util_pct;
    return;
  }
  if (obj.type === 'util_totals') {
    const cores = obj.cores_in_batch || ['CM4', 'CM7'];
    if (!state.utilTotals) state.utilTotals = { CM4: 0, CM7: 0 };
    if (cores.includes('CM4')) state.utilTotals.CM4 = obj.CM4;
    if (cores.includes('CM7')) state.utilTotals.CM7 = obj.CM7;
    renderUtil();
    return;
  }
  if (obj.type === 'PeakMeter') {
    state.peak[obj.channel] = obj.peak_db;
    renderPeak();
    return;
  }
  const text = obj.type === 'decode_error'
    ? ('decode error: ' + obj.error + ' raw=' + obj.raw)
    : (obj.detail || JSON.stringify(obj));
  addRecent(obj.type, text, obj.type === 'decode_error' ? 'error' : '');
}

function connect() {
  eventSource = new EventSource('/events');
  eventSource.onopen = function () {
    statusEl.textContent = 'Connected';
    statusEl.className = 'status connected';
    addRecent('system', 'Connected (SSE).', 'sent');
  };
  eventSource.onerror = function () {
    statusEl.textContent = 'Disconnected';
    statusEl.className = 'status disconnected';
    eventSource.close();
    addRecent('system', 'Connection lost. Reconnecting in 2s...', 'error');
    setTimeout(connect, 2000);
  };
  eventSource.onmessage = function (ev) {
    try {
      onMessage(JSON.parse(ev.data));
    } catch (e) {
      addRecent('—', ev.data, '');
    }
  };
}

async function sendCmd(body) {
  try {
    const r = await fetch('/cmd', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(body)
    });
    const j = await r.json();
    if (j.error) addRecent('cmd', 'Error: ' + j.error, 'error');
  } catch (e) {
    addRecent('cmd', 'Error: ' + e.message, 'error');
  }
}

document.querySelectorAll('.controls button[data-cmd]').forEach(function (btn) {
  btn.onclick = function () {
    sendCmd({ cmd: btn.dataset.cmd });
    addRecent('cmd', '→ ' + btn.dataset.cmd, 'sent');
  };
});
document.getElementById('setGain').onclick = function () {
  const ch = parseInt(document.getElementById('gainCh').value, 10);
  const db = parseFloat(document.getElementById('gainDb').value);
  sendCmd({ cmd: 'set_gain', channel: ch, gain_db: db });
  addRecent('cmd', '→ set_gain ch=' + ch + ' gain_db=' + db, 'sent');
};

renderHeartbeats();
renderUtil();
renderPeak();
connect();

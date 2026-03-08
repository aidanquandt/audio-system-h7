# Host (CLI + Web)

**CLI:** `python tools/host/host.py COM5`  
**Web:** `python tools/host/app.py COM5` → http://127.0.0.1:5000

**Deps (pacman, UCRT64):**  
`pacman -S mingw-w64-ucrt-x86_64-python-flask mingw-w64-ucrt-x86_64-python-pyserial`

## Design

- **Decode path:** Serial bytes → COBS decode → `decode_frame()` (rpc_messages) → typed Python objects → `_message_to_json()` → dict → JSON over SSE. The UI receives structured data (e.g. `obj.type`, `obj.util_pct`) and never parses a display string; protocol is decoded once to types, then passed as data.
- **Backend:** One process owns the port; RX thread decodes and broadcasts to all SSE clients via queues. Commands from the UI go through `/cmd` and are encoded with the same `rpc.make_frame()` / message pack used by the CLI.
- **Frontend:** Single page, SSE for live updates, POST for commands. State is a single object; render functions build tables from state. All device/origin text is escaped before going into the DOM.

| File | Role |
|------|------|
| `host.py` | CLI |
| `app.py` | Web entry: Flask routes, main() |
| `bridge.py` | Serial owner: decode loop, broadcast, send_cmd |
| `templates/index.html` | Page structure |
| `static/style.css` | Layout and theme |
| `static/app.js` | SSE, state, render |
| `rpc_messages.py` | Generated (COBS + pack/unpack); do not edit |

#!/usr/bin/env python3
"""STM32 RPC web host: SSE for live data, POST /cmd for commands. See tools/host/README.md."""

from __future__ import annotations

import queue
import sys
from pathlib import Path

from flask import Flask, render_template, request, Response

from bridge import SerialBridge

_ROOT = Path(__file__).resolve().parent
PORT = sys.argv[1] if len(sys.argv) > 1 else "COM5"
BAUD = 2_000_000

bridge: SerialBridge | None = None

app = Flask(__name__, template_folder=str(_ROOT / "templates"), static_folder=str(_ROOT / "static"))


@app.route("/")
def index():
    return render_template("index.html")


@app.route("/events")
def events():
    if bridge is None:
        return Response("Serial bridge not initialized\n", status=503, mimetype="text/plain")

    def generate():
        q = bridge.add_client()
        try:
            while True:
                try:
                    payload = q.get(timeout=15)
                    yield f"data: {payload}\n\n"
                except queue.Empty:
                    yield ": keepalive\n\n"
        finally:
            bridge.remove_client(q)

    return Response(
        generate(),
        mimetype="text/event-stream",
        headers={"Cache-Control": "no-cache", "X-Accel-Buffering": "no"},
    )


@app.route("/cmd", methods=["POST"])
def cmd():
    if bridge is None:
        return {"ok": False, "error": "Serial bridge not initialized"}, 503
    try:
        body = request.get_json(force=True, silent=True) or {}
    except Exception:
        return {"ok": False, "error": "Invalid JSON"}, 400
    err = bridge.send_cmd(body)
    if err:
        return {"ok": False, "error": err}, 400
    return {"ok": True}


def main():
    global bridge
    print(f"Opening {PORT} at {BAUD}...")
    bridge = SerialBridge(PORT, BAUD)
    try:
        bridge.start()
        print("Serial connected. Starting web server at http://127.0.0.1:5000")
        app.run(host="127.0.0.1", port=5000, debug=False, use_reloader=False)
    except KeyboardInterrupt:
        pass
    finally:
        bridge.stop()
        print("Closed.")


if __name__ == "__main__":
    main()

"""FastAPI application: static files, WebSocket, REST."""

import asyncio
import json
import queue
from pathlib import Path

from fastapi import FastAPI, WebSocket, WebSocketDisconnect
from fastapi.staticfiles import StaticFiles

from host.config import DEFAULT_BAUD_RATE
from host.generated import messaging_protocol_pb2 as pb
from host.protocol.client import ProtocolClient
from host.transports.serial import SerialTransport, list_ports

# Paths: host/src/host/main.py -> host/static
HOST_DIR = Path(__file__).resolve().parent
STATIC_DIR = HOST_DIR.parent.parent / "static"

# Global state: one transport, one client
transport = SerialTransport()
client = ProtocolClient(transport)

# WebSocket broadcast
connections: list[WebSocket] = []
broadcast_queue: queue.Queue = queue.Queue()


def frame_to_json(frame: pb.Frame) -> dict:
    """Convert Frame to JSON-serializable dict for WebSocket."""
    which = frame.WhichOneof("msg")
    if which == "heartbeat":
        return {
            "type": "heartbeat",
            "payload": {
                "uptime_ms": frame.heartbeat.uptime_ms,
                "seq": frame.heartbeat.seq,
            },
        }
    if which == "log_message":
        return {
            "type": "log_message",
            "payload": {
                "timestamp_ms": frame.log_message.timestamp_ms,
                "level": frame.log_message.level,
                "tag": frame.log_message.tag,
                "message": frame.log_message.message,
            },
        }
    if which == "generic_response":
        resp = frame.generic_response
        payload = {
            "request_id": resp.header.request_id,
            "status": pb.StatusCode.Name(resp.header.status),
            "error_msg": resp.header.error_msg,
        }
        if resp.WhichOneof("payload") == "led_status":
            payload["led_status"] = {
                "on": resp.led_status.on,
                "brightness": resp.led_status.brightness,
            }
        return {"type": "generic_response", "payload": payload}
    return {"type": "unknown", "payload": {}}


async def broadcast(message: dict) -> None:
    """Send JSON to all connected WebSocket clients."""
    text = json.dumps(message)
    dead = []
    for ws in connections:
        try:
            await ws.send_text(text)
        except Exception:
            dead.append(ws)
    for ws in dead:
        connections.remove(ws)


def on_frame(frame: pb.Frame) -> None:
    """Called from serial thread when a frame is received. Queues for broadcast."""
    broadcast_queue.put(frame_to_json(frame))


# Set up protocol client callback
client.set_on_frame(on_frame)

app = FastAPI(title="audio-system-h7 Host")


async def broadcast_worker() -> None:
    """Background task: drain queue and broadcast to WebSocket clients."""
    while True:
        try:
            msg = broadcast_queue.get_nowait()
            await broadcast(msg)
        except queue.Empty:
            pass
        await asyncio.sleep(0.01)


@app.on_event("startup")
async def startup() -> None:
    """Start broadcast worker task."""
    asyncio.create_task(broadcast_worker())


@app.get("/ports")
async def get_ports() -> list[dict]:
    """Return list of available serial ports."""
    return list_ports()


@app.websocket("/ws")
async def websocket_endpoint(websocket: WebSocket) -> None:
    """WebSocket handler for bidirectional communication."""
    await websocket.accept()
    connections.append(websocket)
    try:
        while True:
            data = await websocket.receive_text()
            msg = json.loads(data)
            msg_type = msg.get("type", "")
            payload = msg.get("payload", {})

            if msg_type == "connect":
                port = payload.get("port", "")
                if port and not transport.is_connected:
                    try:
                        transport.connect(port, payload.get("baud_rate", DEFAULT_BAUD_RATE))
                        await broadcast({"type": "connected", "payload": {"port": port}})
                    except Exception as e:
                        await broadcast({"type": "error", "payload": {"message": str(e)}})

            elif msg_type == "disconnect":
                if transport.is_connected:
                    transport.disconnect()
                    await broadcast({"type": "disconnected", "payload": {}})

            elif msg_type == "heartbeat":
                if transport.is_connected:
                    client.send_heartbeat()

            elif msg_type == "set_led":
                if transport.is_connected:
                    on = payload.get("on", False)
                    brightness = payload.get("brightness", 100)
                    client.send_set_led(on, brightness)

    except WebSocketDisconnect:
        connections.remove(websocket)
    except Exception:
        if websocket in connections:
            connections.remove(websocket)


# Mount static files last so / catches index.html
if STATIC_DIR.exists():
    app.mount("/", StaticFiles(directory=str(STATIC_DIR), html=True), name="static")

"""Serial port owner: COBS decode loop, broadcast to SSE clients, send commands.

Decode path: bytes → cobs_decode → decode_frame() → typed objects → message_to_json() → dict.
"""

from __future__ import annotations

import json
import queue
import threading

import serial

import rpc_messages as rpc

MSG_NAMES = {cls.MSG_ID: cls.__name__ for cls in rpc.REGISTRY.values()}


def message_to_json(msg_id: int, msg) -> dict:
    """Typed RPC message → dict for JSON transport. UI consumes fields directly."""
    name = MSG_NAMES.get(msg_id, f"0x{msg_id:02x}")
    out = {"type": name, "msg_id": msg_id}
    if msg is None:
        return out
    if isinstance(msg, (rpc.TaskUtilCm4, rpc.TaskUtilCm7)):
        core = "CM4" if isinstance(msg, rpc.TaskUtilCm4) else "CM7"
        name_str = (msg.task_name.split(b"\x00")[0] or b"").decode("ascii", errors="replace")
        out["core"] = core
        out["task_name"] = name_str
        out["util_pct"] = msg.util_pct
    elif isinstance(msg, (rpc.HeartbeatCm4, rpc.HeartbeatCm7)):
        out["seq"] = msg.seq
    elif isinstance(msg, rpc.PeakMeter):
        out["channel"] = msg.channel
        out["peak_db"] = msg.peak_db
    else:
        out["detail"] = str(msg)
    return out


class SerialBridge:
    """Owns the serial port, runs RX decode loop, broadcasts to SSE clients."""

    def __init__(self, port: str, baud: int):
        self._port = port
        self._baud = baud
        self._ser: serial.Serial | None = None
        self._lock = threading.Lock()
        self._client_queues: list[queue.Queue] = []
        self._queues_lock = threading.Lock()
        self._stop = threading.Event()

    def _broadcast(self, obj: dict) -> None:
        with self._queues_lock:
            dead = []
            payload = json.dumps(obj)
            for q in self._client_queues:
                try:
                    q.put_nowait(payload)
                except queue.Full:
                    dead.append(q)
            for q in dead:
                self._client_queues.remove(q)

    def _rx_loop(self) -> None:
        buf = bytearray()
        util_totals: dict[str, int] = {"CM4": 0, "CM7": 0}
        cores_in_batch: set[str] = set()
        while not self._stop.is_set() and self._ser and self._ser.is_open:
            try:
                byte = self._ser.read(1)
            except (serial.SerialException, OSError):
                break
            if not byte:
                continue
            if byte == b"\x00":
                if not buf:
                    continue
                try:
                    raw = rpc.cobs_decode(bytes(buf))
                    msg_id = raw[0]
                    msg = rpc.decode_frame(msg_id, raw[1:])

                    if isinstance(msg, rpc.TaskUtilCm4):
                        cores_in_batch.add("CM4")
                        util_totals["CM4"] += msg.util_pct
                        self._broadcast(message_to_json(msg_id, msg))
                    elif isinstance(msg, rpc.TaskUtilCm7):
                        cores_in_batch.add("CM7")
                        util_totals["CM7"] += msg.util_pct
                        self._broadcast(message_to_json(msg_id, msg))
                    else:
                        if isinstance(msg, rpc.HeartbeatCm4) and "CM4" in cores_in_batch:
                            self._broadcast({
                                "type": "util_totals",
                                "CM4": util_totals["CM4"],
                                "cores_in_batch": ["CM4"],
                            })
                            util_totals["CM4"] = 0
                            cores_in_batch.discard("CM4")
                        elif isinstance(msg, rpc.HeartbeatCm7) and "CM7" in cores_in_batch:
                            self._broadcast({
                                "type": "util_totals",
                                "CM7": util_totals["CM7"],
                                "cores_in_batch": ["CM7"],
                            })
                            util_totals["CM7"] = 0
                            cores_in_batch.discard("CM7")
                        elif cores_in_batch:
                            self._broadcast({
                                "type": "util_totals",
                                "CM4": util_totals["CM4"],
                                "CM7": util_totals["CM7"],
                                "cores_in_batch": list(cores_in_batch),
                            })
                            util_totals["CM4"] = util_totals["CM7"] = 0
                            cores_in_batch = set()
                        self._broadcast(message_to_json(msg_id, msg))
                except Exception as e:
                    self._broadcast({"type": "decode_error", "error": str(e), "raw": bytes(buf).hex()})
                buf.clear()
            else:
                buf.extend(byte)

    def start(self) -> None:
        self._ser = serial.Serial(self._port, self._baud, timeout=0.1)
        self._stop.clear()
        t = threading.Thread(target=self._rx_loop, daemon=True)
        t.start()

    def stop(self) -> None:
        self._stop.set()
        if self._ser:
            try:
                self._ser.close()
            except Exception:
                pass
            self._ser = None

    def add_client(self) -> queue.Queue:
        q = queue.Queue(maxsize=512)
        with self._queues_lock:
            self._client_queues.append(q)
        return q

    def remove_client(self, q: queue.Queue) -> None:
        with self._queues_lock:
            if q in self._client_queues:
                self._client_queues.remove(q)

    def send_cmd(self, cmd: dict) -> str | None:
        with self._lock:
            if not self._ser or not self._ser.is_open:
                return "Serial port not open"
            try:
                kind = cmd.get("cmd")
                if kind == "led_green":
                    m = rpc.LedToggleGreen()
                    self._ser.write(rpc.make_frame(m.MSG_ID, m.pack()))
                elif kind == "led_red":
                    m = rpc.LedToggleRed()
                    self._ser.write(rpc.make_frame(m.MSG_ID, m.pack()))
                elif kind == "set_gain":
                    m = rpc.SetGain(channel=int(cmd.get("channel", 0)), gain_db=float(cmd.get("gain_db", 0)))
                    self._ser.write(rpc.make_frame(m.MSG_ID, m.pack()))
                else:
                    return f"Unknown command: {kind}"
            except Exception as e:
                return str(e)
        return None

#!/usr/bin/env python3
"""
RPC host — decodes COBS frames from the STM32H745 and sends LED toggle commands.

Prints incoming RPC frames (heartbeats, task utilization every ~2s, etc.).
Task utilization lines are shown per task; after each batch a per-core total
is printed so the sum should be 100% per core.

Usage:
    python tools/host/host.py COM5

Keys:
    g  — toggle green LED (CM4 handles locally)
    r  — toggle red LED   (CM4 forwards to CM7 via IPC)
    q  — quit

Install:  pacman -S mingw-w64-ucrt-x86_64-python-pyserial
"""

import sys
import threading
import msvcrt
from pathlib import Path

import serial

# rpc_messages.py lives alongside this script and contains the COBS codec,
# make_frame helper, and all message dataclasses.
sys.path.insert(0, str(Path(__file__).parent))
import rpc_messages as rpc

PORT = sys.argv[1] if len(sys.argv) > 1 else "COM5"
BAUD = 2000000

MSG_NAMES = {cls.MSG_ID: cls.__name__ for cls in rpc.REGISTRY.values()}


def _format_task_util(msg, core: str) -> str:
    """Format task_util_cm4/cm7 for readable utilization output."""
    name_str = (msg.task_name.split(b"\x00")[0] or b"").decode("ascii", errors="replace")
    return f"  {core}  {name_str!r}: {msg.util_pct}%"


def rx_thread(ser: serial.Serial, stop: threading.Event) -> None:
    buf = bytearray()
    util_totals: dict[str, int] = {"CM4": 0, "CM7": 0}
    while not stop.is_set():
        byte = ser.read(1)
        if not byte:
            continue
        if byte == b"\x00":
            if buf:
                try:
                    raw     = rpc.cobs_decode(bytes(buf))
                    msg_id  = raw[0]
                    msg     = rpc.decode_frame(msg_id, raw[1:])
                    name    = MSG_NAMES.get(msg_id, f"0x{msg_id:02x}")

                    if isinstance(msg, rpc.TaskUtilCm4):
                        print(_format_task_util(msg, "CM4"))
                        util_totals["CM4"] += msg.util_pct
                    elif isinstance(msg, rpc.TaskUtilCm7):
                        print(_format_task_util(msg, "CM7"))
                        util_totals["CM7"] += msg.util_pct
                    else:
                        if util_totals["CM4"] > 0 or util_totals["CM7"] > 0:
                            print(f"  (CM4 total: {util_totals['CM4']}%  CM7 total: {util_totals['CM7']}%)")
                        util_totals["CM4"] = util_totals["CM7"] = 0
                        detail = str(msg) if msg else raw[1:].hex()
                        print(f"  [{name}]  {detail}")
                except Exception as e:
                    print(f"  decode error: {e}  raw={buf.hex()}")
                buf.clear()
        else:
            buf.extend(byte)


def run() -> None:
    print(f"Connecting to {PORT} at {BAUD}...")
    with serial.Serial(PORT, BAUD, timeout=0.1) as ser:
        print("Connected.\n")
        print("  g = toggle green LED (CM4)")
        print("  r = toggle red LED   (CM7 via IPC)")
        print("  q = quit")
        print("  (task utilization: CM4/CM7 lines every ~2s)\n")

        stop = threading.Event()
        t = threading.Thread(target=rx_thread, args=(ser, stop), daemon=True)
        t.start()

        while True:
            ch = msvcrt.getch()
            if ch == b"q":
                break
            elif ch == b"g":
                ser.write(rpc.make_frame(rpc.LedToggleGreen.MSG_ID, rpc.LedToggleGreen().pack()))
                print("  -> LED_TOGGLE_GREEN sent")
            elif ch == b"r":
                ser.write(rpc.make_frame(rpc.LedToggleRed.MSG_ID, rpc.LedToggleRed().pack()))
                print("  -> LED_TOGGLE_RED sent")

        stop.set()


if __name__ == "__main__":
    run()

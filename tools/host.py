#!/usr/bin/env python3
"""
RPC host — decodes COBS frames from the STM32H745 and sends LED toggle commands.

Usage:
    python tools/host.py COM5

Keys:
    g  — toggle green LED (CM4 handles locally)
    r  — toggle red LED   (CM4 forwards to CM7 via IPC)
    q  — quit

Install:  pacman -S mingw-w64-ucrt-x86_64-python-pyserial
"""

import sys
import struct
import threading
import msvcrt
import serial

PORT = sys.argv[1] if len(sys.argv) > 1 else "COM5"
BAUD = 2000000

MSG_HEARTBEAT_CM4    = 0x01
MSG_HEARTBEAT_CM7    = 0x02
MSG_LED_TOGGLE_GREEN = 0x10
MSG_LED_TOGGLE_RED   = 0x11
MSG_SET_GAIN         = 0x20
MSG_PEAK_METER       = 0x80

MSG_NAMES = {
    MSG_HEARTBEAT_CM4:    "HEARTBEAT_CM4",
    MSG_HEARTBEAT_CM7:    "HEARTBEAT_CM7",
    MSG_LED_TOGGLE_GREEN: "LED_TOGGLE_GREEN",
    MSG_LED_TOGGLE_RED:   "LED_TOGGLE_RED",
    MSG_SET_GAIN:         "SET_GAIN",
    MSG_PEAK_METER:       "PEAK_METER",
}


def cobs_encode(data: bytes) -> bytes:
    out = bytearray()
    i = 0
    while True:
        block_start = len(out)
        out.append(0)
        code = 1
        while i < len(data) and data[i] != 0x00:
            out.append(data[i])
            i += 1
            code += 1
            if code == 0xFF:
                out[block_start] = code
                code = 1
                block_start = len(out)
                out.append(0)
        out[block_start] = code
        if i >= len(data):
            break
        i += 1
    return bytes(out)


def cobs_decode(data: bytes) -> bytes:
    out = bytearray()
    i = 0
    while i < len(data):
        code = data[i]; i += 1
        if code == 0:
            raise ValueError("unexpected 0x00 inside COBS frame")
        for _ in range(code - 1):
            if i >= len(data):
                raise ValueError("truncated block")
            out.append(data[i]); i += 1
        if code < 0xFF and i < len(data):
            out.append(0x00)
    return bytes(out)


def make_frame(msg_id: int, payload: bytes) -> bytes:
    return cobs_encode(bytes([msg_id]) + payload) + b"\x00"


def parse_payload(msg_id: int, payload: bytes) -> str:
    if msg_id in (MSG_HEARTBEAT_CM4, MSG_HEARTBEAT_CM7) and len(payload) >= 4:
        (seq,) = struct.unpack_from("<I", payload)
        return f"seq={seq}"
    if msg_id == MSG_SET_GAIN and len(payload) >= 5:
        ch, gain = struct.unpack_from("<Bf", payload)
        return f"ch={ch} gain={gain:.2f}dB"
    if msg_id == MSG_PEAK_METER and len(payload) >= 5:
        ch, peak = struct.unpack_from("<Bf", payload)
        return f"ch={ch} peak={peak:.2f}dB"
    return payload.hex() if payload else ""


def rx_thread(ser: serial.Serial, stop: threading.Event) -> None:
    buf = bytearray()
    while not stop.is_set():
        byte = ser.read(1)
        if not byte:
            continue
        if byte == b"\x00":
            if buf:
                try:
                    decoded = cobs_decode(bytes(buf))
                    msg_id  = decoded[0]
                    payload = decoded[1:]
                    name    = MSG_NAMES.get(msg_id, f"0x{msg_id:02x}")
                    detail  = parse_payload(msg_id, payload)
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
        print("  q = quit\n")

        stop = threading.Event()
        t = threading.Thread(target=rx_thread, args=(ser, stop), daemon=True)
        t.start()

        while True:
            ch = msvcrt.getch()
            if ch == b"q":
                break
            elif ch == b"g":
                ser.write(make_frame(MSG_LED_TOGGLE_GREEN, b"\x00"))
                print("  -> LED_TOGGLE_GREEN sent")
            elif ch == b"r":
                ser.write(make_frame(MSG_LED_TOGGLE_RED, b"\x00"))
                print("  -> LED_TOGGLE_RED sent")

        stop.set()


if __name__ == "__main__":
    run()

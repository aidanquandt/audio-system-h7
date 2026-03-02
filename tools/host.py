#!/usr/bin/env python3
"""
Minimal RPC host — decodes COBS frames from the STM32H745 and pretty-prints them.

Usage:
    python host.py COM3          # Windows
    python host.py /dev/ttyUSB0  # Linux/macOS

Install dependency:  pip install pyserial
"""

import sys
import struct
import serial

BAUD = 2000000

MSG_PING       = 0x01
MSG_PONG       = 0x02
MSG_SET_GAIN   = 0x10
MSG_PEAK_METER = 0x80

MSG_NAMES = {
    MSG_PING:       "PING",
    MSG_PONG:       "PONG",
    MSG_SET_GAIN:   "SET_GAIN",
    MSG_PEAK_METER: "PEAK_METER",
}


def cobs_decode(data: bytes) -> bytes:
    out = bytearray()
    i = 0
    while i < len(data):
        code = data[i]
        i += 1
        if code == 0:
            raise ValueError("unexpected 0x00 inside COBS frame")
        for _ in range(code - 1):
            if i >= len(data):
                raise ValueError("truncated block")
            out.append(data[i])
            i += 1
        if code < 0xFF and i < len(data):
            out.append(0x00)
    return bytes(out)


def parse_payload(msg_id: int, payload: bytes) -> str:
    if msg_id == MSG_PING and len(payload) >= 4:
        (seq,) = struct.unpack_from("<I", payload)
        return f"seq={seq}"
    if msg_id == MSG_PONG and len(payload) >= 4:
        (seq,) = struct.unpack_from("<I", payload)
        return f"seq={seq}"
    if msg_id == MSG_SET_GAIN and len(payload) >= 5:
        ch, gain = struct.unpack_from("<Bf", payload)
        return f"ch={ch} gain={gain:.2f}dB"
    if msg_id == MSG_PEAK_METER and len(payload) >= 5:
        ch, peak = struct.unpack_from("<Bf", payload)
        return f"ch={ch} peak={peak:.2f}dB"
    return payload.hex()


def run(port: str) -> None:
    print(f"Connecting to {port} at {BAUD}...")
    with serial.Serial(port, BAUD, timeout=1) as ser:
        print("Connected. Waiting for frames...\n")
        buf = bytearray()
        while True:
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
                        print(f"[{name}]  {detail}")
                    except Exception as e:
                        print(f"decode error: {e}  raw={buf.hex()}")
                    buf.clear()
            else:
                buf.extend(byte)


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python host.py <port>")
        sys.exit(1)
    run(sys.argv[1])

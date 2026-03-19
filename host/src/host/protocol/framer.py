"""Length-delimited frame assembly (mirrors device framer)."""

from enum import IntEnum
from typing import Optional

from host.config import MESSAGING_PROTOCOL_MAX_FRAME_SIZE


class FramerState(IntEnum):
    WAIT_LEN_LO = 0
    WAIT_LEN_HI = 1
    WAIT_PAYLOAD = 2


class Framer:
    """Assembles length-delimited frames from a byte stream."""

    def __init__(self) -> None:
        self._state = FramerState.WAIT_LEN_LO
        self._payload_len = 0
        self._payload_received = 0
        self._payload: bytearray = bytearray(MESSAGING_PROTOCOL_MAX_FRAME_SIZE)

    def feed(self, byte: int) -> Optional[bytes]:
        """
        Feed one byte. Returns complete payload when frame ready, else None.
        Caller must copy payload before next feed().
        """
        b = byte & 0xFF
        if self._state == FramerState.WAIT_LEN_LO:
            self._payload_len = b
            self._state = FramerState.WAIT_LEN_HI
            return None

        if self._state == FramerState.WAIT_LEN_HI:
            self._payload_len |= b << 8
            if self._payload_len > MESSAGING_PROTOCOL_MAX_FRAME_SIZE:
                self.reset()
                return None
            self._payload_received = 0
            self._state = FramerState.WAIT_PAYLOAD
            if self._payload_len == 0:
                self._state = FramerState.WAIT_LEN_LO
                return None
            return None

        if self._state == FramerState.WAIT_PAYLOAD:
            self._payload[self._payload_received] = b
            self._payload_received += 1
            if self._payload_received >= self._payload_len:
                self._state = FramerState.WAIT_LEN_LO
                return bytes(self._payload[: self._payload_len])
            return None

        self.reset()
        return None

    def feed_bytes(self, data: bytes) -> list[bytes]:
        """Feed multiple bytes. Returns list of complete frame payloads."""
        frames: list[bytes] = []
        for b in data:
            frame = self.feed(b)
            if frame is not None:
                frames.append(frame)
        return frames

    def reset(self) -> None:
        """Reset state after consuming a frame or on error."""
        self._state = FramerState.WAIT_LEN_LO
        self._payload_len = 0
        self._payload_received = 0

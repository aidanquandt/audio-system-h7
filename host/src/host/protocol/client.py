"""Protocol client: encode/decode Frame, send/receive."""

import struct
from typing import Callable, Optional

from host.config import MESSAGING_PROTOCOL_MAX_FRAME_SIZE
from host.generated import messaging_protocol_pb2 as pb
from host.protocol.framer import Framer
from host.transports.base import Transport


class ProtocolClient:
    """Wraps transport and framer; encodes/decodes Frame messages."""

    def __init__(self, transport: Transport) -> None:
        self._transport = transport
        self._framer = Framer()
        self._on_frame: Optional[Callable[[pb.Frame], None]] = None
        self._request_id_counter = 1
        transport.set_on_message(self._on_bytes)

    def set_on_frame(self, callback: Callable[[pb.Frame], None]) -> None:
        """Set callback for received frames."""
        self._on_frame = callback

    def send_frame(self, frame: pb.Frame) -> None:
        """Encode frame, add length prefix, send via transport."""
        payload = frame.SerializeToString()
        if len(payload) > MESSAGING_PROTOCOL_MAX_FRAME_SIZE:
            raise ValueError(f"Frame too large: {len(payload)} > {MESSAGING_PROTOCOL_MAX_FRAME_SIZE}")
        length_prefix = struct.pack("<H", len(payload))
        self._transport.send(length_prefix + payload)

    def send_heartbeat(self, uptime_ms: int = 0, seq: int = 0) -> None:
        """Send a Heartbeat frame."""
        frame = pb.Frame()
        frame.heartbeat.uptime_ms = uptime_ms
        frame.heartbeat.seq = seq
        self.send_frame(frame)

    def send_set_led(self, on: bool, brightness: int = 100) -> int:
        """Send SetLed via GenericRequest. Returns request_id."""
        request_id = self._request_id_counter
        self._request_id_counter = (self._request_id_counter % 0xFFFF) + 1

        frame = pb.Frame()
        frame.generic_request.header.request_id = request_id
        frame.generic_request.set_led.on = on
        frame.generic_request.set_led.brightness = min(100, max(0, brightness))
        self.send_frame(frame)
        return request_id

    def _on_bytes(self, data: bytes) -> None:
        """Called by transport when bytes received."""
        for payload in self._framer.feed_bytes(data):
            try:
                frame = pb.Frame()
                frame.ParseFromString(payload)
                if self._on_frame:
                    self._on_frame(frame)
            except Exception:
                pass  # Ignore decode errors

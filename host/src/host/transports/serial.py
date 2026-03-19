"""Serial transport using pyserial."""

import threading
from typing import Callable, Optional

import serial
import serial.tools.list_ports

from host.config import DEFAULT_BAUD_RATE
from host.transports.base import Transport


class SerialTransport(Transport):
    """Serial port transport implementation."""

    def __init__(self) -> None:
        self._serial: Optional[serial.Serial] = None
        self._on_message: Optional[Callable[[bytes], None]] = None
        self._reader_thread: Optional[threading.Thread] = None
        self._stop_event = threading.Event()

    def connect(self, port: str, baud_rate: int = DEFAULT_BAUD_RATE) -> None:
        """Open serial port and start read loop."""
        if self._serial:
            raise RuntimeError("Already connected")
        self._serial = serial.Serial(port=port, baudrate=baud_rate)
        self._stop_event.clear()
        self._reader_thread = threading.Thread(target=self._read_loop, daemon=True)
        self._reader_thread.start()

    def disconnect(self) -> None:
        """Close serial port and stop read loop."""
        self._stop_event.set()
        if self._reader_thread:
            self._reader_thread.join(timeout=5.0)
            self._reader_thread = None
        if self._serial:
            self._serial.close()
            self._serial = None

    def send(self, data: bytes) -> None:
        """Send raw bytes to the device."""
        if not self._serial or not self._serial.is_open:
            raise RuntimeError("Not connected")
        self._serial.write(data)

    def set_on_message(self, callback: Callable[[bytes], None]) -> None:
        """Set callback for received bytes."""
        self._on_message = callback

    @property
    def is_connected(self) -> bool:
        """Return True if connected."""
        return self._serial is not None and self._serial.is_open

    def _read_loop(self) -> None:
        """Background thread: read bytes and invoke callback."""
        while not self._stop_event.is_set() and self._serial and self._serial.is_open:
            try:
                if self._serial.in_waiting:
                    data = self._serial.read(self._serial.in_waiting)
                    if self._on_message and data:
                        self._on_message(data)
                else:
                    self._stop_event.wait(timeout=0.01)
            except (serial.SerialException, OSError):
                break


def list_ports() -> list[dict[str, str]]:
    """Return list of available serial ports."""
    ports = []
    for p in serial.tools.list_ports.comports():
        ports.append({
            "device": p.device,
            "description": p.description or "",
            "hwid": p.hwid or "",
        })
    return ports

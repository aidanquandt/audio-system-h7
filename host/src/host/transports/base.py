"""Transport layer abstraction."""

from abc import ABC, abstractmethod
from typing import Callable


class Transport(ABC):
    """Abstract base for transport implementations."""

    @abstractmethod
    def connect(self, port: str, baud_rate: int = 2000000) -> None:
        """Connect to the device."""
        ...

    @abstractmethod
    def disconnect(self) -> None:
        """Disconnect from the device."""
        ...

    @abstractmethod
    def send(self, data: bytes) -> None:
        """Send raw bytes to the device."""
        ...

    @abstractmethod
    def set_on_message(self, callback: Callable[[bytes], None]) -> None:
        """Set callback for received bytes."""
        ...

    @property
    @abstractmethod
    def is_connected(self) -> bool:
        """Return True if connected."""
        ...

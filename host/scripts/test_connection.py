#!/usr/bin/env python3
"""
Quick test: connect to device, send SetLed, wait for GenericResponse.
Usage: cd host && python scripts/test_connection.py /dev/ttyACM0
(Requires: pip install -e .)
"""

import sys
import time

from host.transports.serial import SerialTransport, list_ports
from host.protocol.client import ProtocolClient
from host.generated import messaging_protocol_pb2 as pb

def main():
    if len(sys.argv) < 2:
        print("Available ports:")
        for p in list_ports():
            print(f"  {p['device']}: {p['description']}")
        print("\nUsage: python scripts/test_connection.py <port>")
        print("Example: python scripts/test_connection.py /dev/ttyACM0")
        sys.exit(1)

    port = sys.argv[1]
    received = []

    def on_frame(frame):
        received.append(frame)

    transport = SerialTransport()
    client = ProtocolClient(transport)
    client.set_on_frame(on_frame)

    print(f"Connecting to {port} at 2000000 baud...")
    try:
        transport.connect(port)
    except Exception as e:
        print(f"Connect failed: {e}")
        sys.exit(1)

    print("Sending SetLed(on=True, brightness=50)...")
    client.send_set_led(on=True, brightness=50)

    print("Waiting up to 5s for response...")
    for _ in range(50):
        time.sleep(0.1)
        if received:
            break

    transport.disconnect()

    if received:
        frame = received[0]
        which = frame.WhichOneof("msg")
        if which == "generic_response":
            r = frame.generic_response
            print(f"OK: Got GenericResponse request_id={r.header.request_id} status={r.header.status}")
            if r.WhichOneof("payload") == "led_status":
                print(f"     LedStatus: on={r.led_status.on} brightness={r.led_status.brightness}")
        else:
            print(f"Got unexpected message type: {which}")
    else:
        print("No response received. Check:")
        print("  - Device is flashed and running")
        print("  - Correct port (USART3 on PB10/PB11)")
        print("  - Baud rate 2000000 supported by adapter")
        print("  - TX/RX not swapped")
        sys.exit(1)

if __name__ == "__main__":
    main()

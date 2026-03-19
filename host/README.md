# audio-system-h7 Host

Python backend for communicating with the audio-system-h7 device over serial.

## Setup

```bash
cd host
pip install -e .
```

Or from project root:

```bash
pip install -e host/
```

## Run

```bash
cd host
uvicorn host.main:app --reload --host 0.0.0.0 --port 8000
```

Then open http://localhost:8000 in a browser.

## Usage

1. **Connect**: Select a serial port from the dropdown and click Connect.
2. **LED Control**: Toggle the checkbox or move the brightness slider. This sends a `SetLed` request; the device responds with `LedStatus`. **This is the only action that triggers a device response.**
3. **Heartbeat**: "Send Heartbeat" sends a frame to the device. The device receives it but does not echo it back.
4. **Logs**: Device log messages appear here when the device sends them (not yet wired in firmware).

## Testing

**Expected behavior when it works:**

- Toggle LED On/Off → green LED on the board should turn on/off, and "Status" should update with the response.
- Change brightness → "Status" should show the new brightness.

**If you see no response:**

1. **LED control is the only thing that triggers a response** – Try toggling the LED checkbox or moving the brightness slider.
2. **Baud rate**: Device uses 2 Mbps (2000000). Ensure your USB-serial adapter supports this. Many FTDI/CP2102 adapters do; some cheaper ones cap at 921600.
3. **Correct port**: USART3 is on PB10 (TX) / PB11 (RX). On Nucleo boards, the ST-Link often exposes a second VCP connected to these pins – that may appear as `/dev/ttyACM0` or similar.
4. **Firmware running**: Flash the device with `make flash` and ensure it’s running (e.g. heartbeat LED blinking).
5. **Wiring**: If using an external USB-serial adapter, connect its TX to board RX (PB11) and RX to board TX (PB10).

**Command-line test** (no browser):

```bash
cd host
python scripts/test_connection.py /dev/ttyACM0
```

This sends a SetLed request and reports whether a response was received.

## Port Selection

- **Linux**: `/dev/ttyUSB0`, `/dev/ttyACM0` (USB-serial adapter or CDC)
- **macOS**: `/dev/cu.usbserial-*`
- **Windows**: `COM3`, `COM4`, etc.

The device uses 2000000 baud (2 Mbps).

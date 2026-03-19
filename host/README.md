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

1. Connect to the device: select a serial port from the dropdown and click Connect.
2. Send Heartbeat: click to send a heartbeat frame to the device.
3. Logs: device log messages appear in the Logs section.
4. LED Control: toggle and brightness slider send SetLed requests to the device.

## Port Selection

- **Linux**: `/dev/ttyUSB0`, `/dev/ttyACM0` (USB-serial adapter or CDC)
- **macOS**: `/dev/cu.usbserial-*`
- **Windows**: `COM3`, `COM4`, etc.

The device uses 2000000 baud (2 Mbps). Ensure your USB-serial adapter supports this rate.

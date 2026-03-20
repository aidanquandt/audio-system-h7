# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

STM32H745 dual-core audio system with a Python host communicating over UART. The MCU runs FreeRTOS with two cores (CM4 handles application logic, CM7 handles system init), connected to a FastAPI web app via a length-delimited protobuf messaging protocol at 2 Mbps.

## Setup

Initialize submodules on fresh clone:
```bash
git submodule update --init --recursive
```

Python dependencies:
```bash
pip install 'protobuf>=4.24.0' grpcio-tools
```

## Build Commands

```bash
make build              # Build both CM4 and CM7 cores
make codegen            # Regenerate C protobuf code from .proto (run after proto changes)
make host-codegen       # Regenerate Python protobuf code
make flash              # Flash both cores via OpenOCD
make build-and-flash    # Build then flash
make rebuild            # Clean, configure, and build from scratch
make host-run           # Start host webapp (uvicorn on :8000)
make host-install       # Install host Python package
```

Build artifacts are at `build/CM4/stm32_hw_CM4.elf`, `build/CM7/stm32_hw_CM7.elf`, and `build/stm32_hw_merged.hex`.

## Architecture

### Dual-Core Split
- **CM7**: System initialization only (clocks, memory, boots CM4)
- **CM4**: All application logic — FreeRTOS tasks, protocol stack, peripherals

### Messaging Protocol Stack (bottom-up)
1. **Transport** (`src/messaging_protocol/messaging_protocol_uart.c`) — UART on USART3 (PB10/PB11, 2 Mbps), 128B packets, FreeRTOS StreamBuffer for RX
2. **Framer** (`src/messaging_protocol/messaging_protocol_framer.c`) — Length-delimited framing state machine (1024B max frame)
3. **Protocol task** (`src/messaging_protocol/messaging_protocol_task.c`) — FreeRTOS RX loop, dispatches decoded frames
4. **Handler** (`src/messaging_protocol/messaging_protocol_handler.c`) — Routes `Frame` oneof to domain handlers (LED, audio, etc.)

The transport layer is abstracted — UART is the current impl, CDC/WiFi are planned future transports.

### Protocol Definition
Single source of truth: `messaging_protocol/messaging_protocol.proto`. Nanopb encodes on the MCU side; Python protobuf on the host side. Run `make codegen` and `make host-codegen` after any `.proto` changes.

Generated files:
- `generated/` — C nanopb files (committed, regenerate with `make codegen`)
- `host/src/host/generated/` — Python protobuf files

### Application Components (`src/app_main/`)
- `led/` — LED control (CM4); CM7 has a stub
- `display/` — LCD interface
- `heartbeat/` — Periodic heartbeat FreeRTOS task
- `dlog/` — Debug logging (sent as `LogMessage` frames to host)
- `touch/` — Touch input

### Host Application (`host/`)
FastAPI server with serial transport, protobuf framing/parsing, and a browser UI at `http://localhost:8000`. WebSocket streams real-time updates to the browser.

## Code Formatting

C code uses clang-format (`.clang-format` at repo root). The project uses clangd for IntelliSense — run `make build` to generate `build/compile_commands.json`, then `scripts/merge-compile-commands.sh` merges both cores' compile commands for IDE use.

## Adding New Protocol Messages

1. Add message types to `messaging_protocol/messaging_protocol.proto`
2. Run `make codegen` (C) and `make host-codegen` (Python)
3. Add a handler case in `src/messaging_protocol/messaging_protocol_handler.c`
4. Implement the domain handler in the appropriate `src/app_main/` subdirectory
5. Add corresponding host-side handling in `host/src/host/`

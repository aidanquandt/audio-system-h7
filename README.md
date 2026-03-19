# audio-system-h7

STM32H745 dual-core audio system with messaging protocol over UART.

## Build

New clones must initialize submodules before building:

```bash
git submodule update --init --recursive
```

Then:

```bash
make build
```

Codegen (run after proto changes):

```bash
make codegen
```

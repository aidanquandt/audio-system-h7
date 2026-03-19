# audio-system-h7

STM32H745 dual-core audio system with messaging protocol over UART.

## Build

New clones must initialize submodules before building:

```bash
git submodule update --init --recursive
```

Python dependencies for codegen (nanopb generator requires protobuf 4.x+):

```bash
pip install 'protobuf>=4.24.0'
```

For host codegen (`make host-codegen`), also install grpcio-tools:

```bash
pip install grpcio-tools
```

Then:

```bash
make build
```

Codegen (run after proto changes):

```bash
make codegen
```

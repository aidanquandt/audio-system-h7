# STM32H745 Audio System

Audio and DSP project for STM32H745 Discovery board (dual-core Cortex-M4/M7).

## Project Structure

```
├── .vscode/        # VSCode tasks and settings
├── cube/           # STM32CubeMX generated code
│   ├── CM4/        # Cortex-M4 core code
│   ├── CM7/        # Cortex-M7 core code (main)
│   ├── Common/     # Shared resources
│   └── Drivers/    # HAL drivers
├── src/            # Application code
├── include/        # Application headers
├── lib/            # Third-party libraries (CMSIS-DSP, etc.)
├── tests/          # Unit tests
├── tools/          # Build scripts (bash)
└── build/          # Build output (gitignored)
    ├── CM4/        # CM4 build artifacts
    └── CM7/        # CM7 build artifacts
```

## Build System

This project uses **bash scripts** for dual-core orchestration. Each core builds independently to avoid CMake target name conflicts.

### Quick Start

```bash
# Just build (auto-configures if needed)
./tools/build.sh

# Or configure explicitly first
./tools/configure.sh
./tools/build.sh
```

### Build Scripts

- `configure.sh` - Configure both CM4 and CM7
- `build.sh` - Build both cores
- `clean.sh` - Remove build artifacts
- `rebuild.sh` - Clean + configure + build

### VSCode Integration

**Keyboard shortcuts:**
- `Ctrl+Shift+B` - Build both cores (default task)
- `Ctrl+Shift+P` → "Run Task" - See all build options

**Available tasks:**
- Build Both Cores (default)
- Build CM4 Only
- Build CM7 Only
- Configure Both Cores
- Clean Build
- Full Rebuild

### Command Line Usage

```bash
# Configure and build everything
./tools/rebuild.sh

# Build only one core
cmake --build build/CM4 -j8
cmake --build build/CM7 -j8

# Parallel jobs (default: 8)
JOBS=16 ./tools/build.sh
```

## Development Workflow

### Making Changes

1. **CubeMX regeneration**: Regenerate into `cube/` - scripts unaffected
2. **Application code**: Edit in `src/` and `include/`
3. **Add source files**: Edit `cube/CM4/CMakeLists.txt` or `cube/CM7/CMakeLists.txt`

### Typical Day

```bash
# Morning: Pull and build
git pull
./tools/rebuild.sh

# During development: Quick iterations
# Edit code → Ctrl+Shift+B (or ./tools/build.sh)

# Before commit: Clean build
./tools/rebuild.sh
```

## Hardware

- **Board**: STM32H745I-DISCO
- **MCU**: STM32H745XIH6
  - Cortex-M7 @ 480MHz (DSP, main processing)
  - Cortex-M4 @ 240MHz (peripherals, I/O)
- **RAM**: 1MB
- **Flash**: 2MB

## Build Outputs

After building:
- `build/CM4/cube_CM4.elf` - CM4 firmware
- `build/CM7/cube_CM7.elf` - CM7 firmware

## Why Bash Scripts?

Senior engineers prefer this approach because:
- ✅ **Simple**: No fighting CubeMX-generated CMake files
- ✅ **Maintainable**: Scripts don't break on CubeMX regeneration
- ✅ **Cross-platform**: Works on Windows (msys2), Linux, macOS
- ✅ **CI/CD ready**: Easy to automate
- ✅ **Clean separation**: Each core builds independently

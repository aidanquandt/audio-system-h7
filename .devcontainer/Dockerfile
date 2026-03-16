FROM mcr.microsoft.com/devcontainers/base:ubuntu
RUN apt-get update && apt-get install -y \
    gcc-arm-none-eabi \
    libnewlib-arm-none-eabi \
    build-essential \
    cmake \
    ninja-build \
    git \
    clangd \
    openocd \
    libusb-1.0-0 \
    usbutils \
    jq \
    python3 \
    && rm -rf /var/lib/apt/lists/*

# Grant permissions to ST-Link V3 (0483:374e)
RUN mkdir -p /etc/udev/rules.d && \
    echo 'SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374e", MODE="0666"' > /etc/udev/rules.d/99-stlink.rules
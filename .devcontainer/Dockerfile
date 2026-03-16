FROM mcr.microsoft.com/devcontainers/base:ubuntu
RUN apt-get update && apt-get install -y \
    gcc-arm-none-eabi \
    libnewlib-arm-none-eabi \
    build-essential \
    cmake \
    ninja-build \
    git \
    python3 \
    && rm -rf /var/lib/apt/lists/*
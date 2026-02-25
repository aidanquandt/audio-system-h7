cmake_minimum_required(VERSION 3.22)

project(stm32h745-audio 
    LANGUAGES C CXX ASM
    VERSION 1.0.0
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# You can build either CM4 or CM7 core
# Default to CM7 (main core for DSP)
option(BUILD_CM4 "Build for Cortex-M4 core" OFF)

if(BUILD_CM4)
    add_subdirectory(cube/CM4)
else()
    add_subdirectory(cube/CM7)
endif()

# To build CM7 (default): cmake -B build
# To build CM4: cmake -B build -DBUILD_CM4=ON

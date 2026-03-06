# src/CMakeLists.txt — unified application library for the STM32H745 dual-core project.
#
# The parent CMakeLists must set APP_CORE to either "CM4" or "CM7" before calling
# add_subdirectory() on this directory, e.g.:
#
#   set(APP_CORE CM4)
#   add_subdirectory(${CMAKE_SOURCE_DIR}/../../src ${CMAKE_BINARY_DIR}/app)
#
if(NOT DEFINED APP_CORE)
    message(FATAL_ERROR "APP_CORE must be set to CM4 or CM7 before adding src/")
endif()

string(TOLOWER "${APP_CORE}" _core_lower)
set(_target app_${_core_lower})

# ── Sources compiled for every core (display/sdram/transport use CORE_CM4 stubs on CM7) ──
set(_sources
    app_main/app_main.c
    display/display.c
    heartbeat/heartbeat.c
    led/led.c
    rpc/rpc.c
    sdram/sdram.c
    transport/transport.c
)

# ──────────────────────────────────────────────────────────────────────────────

add_library(${_target} STATIC ${_sources})

target_include_directories(${_target}
    PUBLIC
        # Repo root so includes use top-level folder: #include "src/display/display.h", "generated/...", "bsp/...", "drivers/..."
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${_target} PRIVATE bsp drivers generated freertos_iface project_includes cobs)

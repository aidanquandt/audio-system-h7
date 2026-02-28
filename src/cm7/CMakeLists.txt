# CM7 application library. Sees only the BSP API and FreeRTOS — never the HAL.
add_library(app_cm7 STATIC
    app_main/app_main.c
)

target_include_directories(app_cm7
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}           # exposes app_main.h to the executable
        ${CMAKE_CURRENT_SOURCE_DIR}/../common
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_link_libraries(app_cm7 PRIVATE bsp ipc_common freertos_iface)

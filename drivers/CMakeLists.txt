# Driver layer: wraps BSP with RTOS/serialisation (e.g. LCD DMA2D fill).
# Only CM4 builds the LCD driver; other cores get stubs.
add_library(drivers STATIC lcd_driver.c)

target_include_directories(drivers
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..   # consumers: #include "drivers/lcd_driver.h"
)

target_link_libraries(drivers PRIVATE bsp freertos_iface)

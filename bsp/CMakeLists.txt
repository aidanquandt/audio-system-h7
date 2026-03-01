# BSP: the only target permitted to call the HAL directly.
# stm32cubemx is PRIVATE — HAL headers do not propagate to consumers.
add_library(bsp STATIC
    gpio.c
    hsem.c
    uart.c
)

target_include_directories(bsp
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/..   # consumers: #include "bsp/gpio.h"
)

target_link_libraries(bsp PRIVATE stm32cubemx)

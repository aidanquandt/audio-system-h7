# BSP: the only target permitted to call the HAL directly.
# stm32cubemx is PRIVATE — HAL headers do not propagate to consumers.
add_library(bsp STATIC
    gpio/gpio.c
    hsem/hsem.c
    lcd/lcd.c
    sdram/sdram.c
    uart/uart.c
)

target_include_directories(bsp
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/..   # consumers: #include "bsp/gpio/gpio.h"
)

target_link_libraries(bsp PRIVATE stm32cubemx)

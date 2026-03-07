# Driver layer: HAL + RTOS. Only layer that links to stm32cubemx.
# Only CM4 builds LCD/touch/uart/sdram/i2c; other cores get stubs.
add_library(drivers STATIC
    gpio/gpio.c
    hsem/hsem.c
    i2c/i2c.c
    lcd/lcd.c
    sdram/sdram.c
    touch/touch.c
    uart/uart.c
)

target_include_directories(drivers
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..   # consumers: #include "drivers/..."
)

target_link_libraries(drivers PRIVATE stm32cubemx freertos_iface)

# Driver layer: HAL + RTOS. Only layer that links to stm32cubemx.
# Only CM4 builds LCD/touch/uart/sdram/i2c; other cores get stubs.
add_library(drivers STATIC
    gpio/gpio_driver.c
    hsem/hsem_driver.c
    i2c/i2c_driver.c
    lcd/lcd_driver.c
    sdram/sdram_driver.c
    touch/touch_driver.c
    uart/uart_driver.c
)

target_include_directories(drivers
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..   # consumers: #include "drivers/..."
)

target_link_libraries(drivers PRIVATE stm32cubemx freertos_iface)

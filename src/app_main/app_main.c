#include "app_main/app_main.h"
#include "ipc/ipc.h"
#include "heartbeat/heartbeat.h"

#ifdef CORE_CM4
#include "uart/uart.h"
#include "bsp/gpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"

static void uart_rx_task(void *arg)
{
    (void)arg;
    uint8_t buf[UART_RX_BUF_LEN];
    StreamBufferHandle_t stream = uart_get_rx_stream();

    for (;;) {
        size_t n = xStreamBufferReceive(stream, buf, sizeof(buf), portMAX_DELAY);
        if (n > 0) {
            /* TODO: feed buf[0..n] into protocol parser */
            bsp_gpio_toggle(BSP_GPIO_LED_GREEN);
        }
    }
}
#endif

void app_main(void)
{
    ipc_init();
#ifdef CORE_CM4
    uart_init();
    xTaskCreate(uart_rx_task, "uart_rx", 256, NULL, tskIDLE_PRIORITY + 2, NULL);
#endif
    heartbeat_init();
}

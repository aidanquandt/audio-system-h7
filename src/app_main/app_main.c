#include "app_main/app_main.h"
#include "ipc/ipc.h"
#include "heartbeat/heartbeat.h"

#ifdef CORE_CM4
#include "uart/uart.h"
#endif

void app_main(void)
{
    ipc_init();
#ifdef CORE_CM4
    uart_init();
#endif
    heartbeat_init();
}

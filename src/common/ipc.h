#pragma once

#include "ipc_shared.h"

void ipc_init(void);
void ipc_send(ipc_cmd_t cmd, uint32_t arg0, uint32_t arg1, uint32_t arg2);

/* Override in application code to handle incoming messages for this core. */
void ipc_on_message(const ipc_msg_t *msg);

#pragma once

#include <stdint.h>

typedef void (*hsem_driver_callback_t)(uint32_t sem_mask);

void hsem_driver_init(void);
void hsem_driver_notify(uint32_t channel);
void hsem_driver_arm(uint32_t channel_mask);
void hsem_driver_register_callback(uint32_t channel_mask, hsem_driver_callback_t cb);

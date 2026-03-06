#pragma once

#include <stdint.h>

typedef void (*bsp_hsem_callback_t)(uint32_t sem_mask);

void bsp_hsem_init(void);
void bsp_hsem_notify(uint32_t channel);
void bsp_hsem_arm(uint32_t channel_mask);
void bsp_hsem_register_callback(uint32_t channel_mask, bsp_hsem_callback_t cb);

#pragma once

/* System-wide HSEM channel assignments for the STM32H745 (channels 0–31).
 *
 * Single authoritative table of inter-core signalling resources.
 * Add new channels here; never define them inside a module's own config. */

#define HSEM_CH_RPC_CM4_TO_CM7  0U  /* rpc: CM4 notifies CM7 of a queued frame */
#define HSEM_CH_RPC_CM7_TO_CM4  1U  /* rpc: CM7 notifies CM4 of a queued frame */

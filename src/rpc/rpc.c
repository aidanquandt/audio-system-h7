#include "rpc/rpc.h"
#include "cobs/cobs.h"
#include "transport/transport.h"
#include "ipc/ipc_shared.h"
#include "bsp/hsem.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <string.h>

#define MAX_HANDLERS       16U
#define IPC_BOOT_TIMEOUT_MS 5000U

/* Raw COBS bytes fit in this; caller synthesises the [msg_id | payload] before encoding. */
#define COBS_FRAME_BUF_LEN  COBS_ENCODED_LEN(1U + RPC_FRAME_MAX_PAYLOAD)

typedef struct {
    uint8_t        msg_id;
    rpc_dest_t     dest;
    rpc_handler_fn fn;
} handler_entry_t;

static handler_entry_t    s_handlers[MAX_HANDLERS];
static size_t             s_handler_count = 0;
static SemaphoreHandle_t  s_ipc_wakeup;

static void rpc_dispatch(uint8_t msg_id, const uint8_t *payload, size_t len);

/* ── IPC queue helpers (mirrors ipc.c pattern) ───────────────────────────── */

static int rpc_frame_push(volatile rpc_frame_queue_t *q, const rpc_frame_t *f)
{
    uint32_t head = q->head;
    uint32_t next = (head + 1U) & (RPC_QUEUE_DEPTH - 1U);
    if (next == q->tail) return -1;
    q->slots[head] = *f;
    __DMB();
    q->head = next;
    return 0;
}

static int rpc_frame_pop(volatile rpc_frame_queue_t *q, rpc_frame_t *f)
{
    uint32_t tail = q->tail;
    if (tail == q->head) return -1;
    *f = q->slots[tail];
    __DMB();
    q->tail = (tail + 1U) & (RPC_QUEUE_DEPTH - 1U);
    return 0;
}

/* ── HSEM callback (ISR context) ─────────────────────────────────────────── */

static void on_ipc_hsem(uint32_t sem_mask)
{
    (void)sem_mask;
    BaseType_t woken = pdFALSE;
    xSemaphoreGiveFromISR(s_ipc_wakeup, &woken);
    portYIELD_FROM_ISR(woken);
}

/* ── Wire RX task — CM4 only, one instance per registered transport ───────── */

#ifdef CORE_CM4
static void wire_task(void *arg)
{
    const transport_t   *t      = (const transport_t *)arg;
    StreamBufferHandle_t stream  = t->get_rx_stream();
    uint8_t raw[COBS_FRAME_BUF_LEN];
    uint8_t dec[1U + RPC_FRAME_MAX_PAYLOAD];
    size_t  n = 0;

    for (;;) {
        uint8_t b;
        xStreamBufferReceive(stream, &b, 1, portMAX_DELAY);

        if (b == 0x00) {
            if (n > 0) {
                int len = cobs_decode(raw, n, dec);
                if (len > 0) {
                    rpc_dispatch(dec[0], dec + 1, (size_t)(len - 1));
                }
                n = 0;
            }
        } else {
            if (n < sizeof(raw)) {
                raw[n++] = b;
            } else {
                n = 0; /* overrun — drop and wait for next 0x00 */
            }
        }
    }
}
#endif /* CORE_CM4 */

/* ── IPC RX task — both cores ────────────────────────────────────────────── */

static void ipc_rx_task(void *arg)
{
    (void)arg;

#ifdef CORE_CM4
    volatile rpc_frame_queue_t *q       = &IPC_SHARED->cm7_to_cm4_rpc;
    uint32_t                    channel = HSEM_CH_RPC_CM7_TO_CM4;

    uint32_t wait = 0;
    while (IPC_SHARED->ready_flag != IPC_READY_FLAG) {
        vTaskDelay(pdMS_TO_TICKS(1));
        if (++wait > IPC_BOOT_TIMEOUT_MS) break;
    }
    __DMB();
#else
    volatile rpc_frame_queue_t *q       = &IPC_SHARED->cm4_to_cm7_rpc;
    uint32_t                    channel = HSEM_CH_RPC_CM4_TO_CM7;
#endif

    bsp_hsem_register_callback(1U << channel, on_ipc_hsem);
    bsp_hsem_arm(1U << channel);

    for (;;) {
        xSemaphoreTake(s_ipc_wakeup, pdMS_TO_TICKS(50));

        rpc_frame_t frame;
        while (rpc_frame_pop(q, &frame) == 0) {
#ifdef CORE_CM4
            /* Forward CM7-originated messages to the host over the wire. */
            uint8_t buf[1U + RPC_FRAME_MAX_PAYLOAD];
            uint8_t encoded[COBS_FRAME_BUF_LEN + 1U];
            buf[0] = frame.msg_id;
            memcpy(buf + 1, frame.data, frame.len);
            size_t elen = cobs_encode(buf, 1U + frame.len, encoded);
            encoded[elen] = 0x00;
            transport_send(encoded, elen + 1U);
#else
            rpc_dispatch(frame.msg_id, frame.data, frame.len);
#endif
        }
    }
}

/* ── Dispatch ────────────────────────────────────────────────────────────── */

static void rpc_dispatch(uint8_t msg_id, const uint8_t *payload, size_t len)
{
    for (size_t i = 0; i < s_handler_count; i++) {
        if (s_handlers[i].msg_id != msg_id) continue;

#ifdef CORE_CM4
        if (s_handlers[i].dest == DEST_CM7) {
            rpc_frame_t frame = { .msg_id = msg_id, .len = (uint8_t)len };
            configASSERT(len <= RPC_FRAME_MAX_PAYLOAD);
            memcpy((void *)frame.data, payload, len);

            taskENTER_CRITICAL();
            int rc = rpc_frame_push(&IPC_SHARED->cm4_to_cm7_rpc, &frame);
            taskEXIT_CRITICAL();

            if (rc == 0) {
                bsp_hsem_notify(HSEM_CH_RPC_CM4_TO_CM7);
            }
        } else {
            if (s_handlers[i].fn) s_handlers[i].fn(msg_id, payload, len);
        }
#else
        if (s_handlers[i].fn) s_handlers[i].fn(msg_id, payload, len);
#endif
        return; /* first matching entry wins */
    }
}

/* ── Public API ──────────────────────────────────────────────────────────── */

void rpc_register(uint8_t msg_id, rpc_dest_t dest, rpc_handler_fn fn)
{
    configASSERT(s_handler_count < MAX_HANDLERS);
    s_handlers[s_handler_count++] = (handler_entry_t){
        .msg_id = msg_id,
        .dest   = dest,
        .fn     = fn,
    };
}

void rpc_transmit(uint8_t msg_id, const void *payload, size_t len)
{
    configASSERT(len <= RPC_FRAME_MAX_PAYLOAD);

#ifdef CORE_CM4
    uint8_t buf[1U + RPC_FRAME_MAX_PAYLOAD];
    uint8_t encoded[COBS_FRAME_BUF_LEN + 1U];
    buf[0] = msg_id;
    memcpy(buf + 1, payload, len);
    size_t elen = cobs_encode(buf, 1U + len, encoded);
    encoded[elen] = 0x00;
    transport_send(encoded, elen + 1U);
#else
    rpc_frame_t frame = { .msg_id = msg_id, .len = (uint8_t)len };
    memcpy(frame.data, payload, len);

    taskENTER_CRITICAL();
    int rc = rpc_frame_push(&IPC_SHARED->cm7_to_cm4_rpc, &frame);
    taskEXIT_CRITICAL();

    if (rc == 0) {
        bsp_hsem_notify(HSEM_CH_RPC_CM7_TO_CM4);
    }
#endif
}

void rpc_init(void)
{
    s_ipc_wakeup = xSemaphoreCreateBinary();
    configASSERT(s_ipc_wakeup);

    xTaskCreate(ipc_rx_task, "rpc_ipc_rx", 512, NULL, tskIDLE_PRIORITY + 3U, NULL);

#ifdef CORE_CM4
    for (size_t i = 0; i < transport_count(); i++) {
        xTaskCreate(wire_task, "rpc_wire", 512, (void *)transport_get(i),
                    tskIDLE_PRIORITY + 2U, NULL);
    }
#endif
}

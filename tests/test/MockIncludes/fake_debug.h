#ifndef _MOCK_DEBUG_H
#define _MOCK_DEBUG_H
#include "fake_queue.h"
#include "fake_hal_defs.h"

// Function Prototypes
HAL_StatusTypeDef debugInit();



// Internal Config
extern QueueHandle_t printQueue;

#define UART_PRINT_TIMEOUT 100

#if BOARD_TYPE_F7
// We have more space on F7, so make this longer
#define PRINT_QUEUE_LENGTH 15
#elif BOARD_TYPE_F0
#define PRINT_QUEUE_LENGTH 5
#endif

#define PRINT_QUEUE_STRING_SIZE 100
#define PRINT_QUEUE_SEND_TIMEOUT_TICKS  10

#define UART_RX_QUEUE_LENGTH 100
#endif

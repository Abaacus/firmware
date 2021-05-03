#include "fake_debug.h"
#include "stddef.h"
#include "fake_hal_defs.h"
QueueHandle_t printQueue;
QueueHandle_t uartRxQueue;

HAL_StatusTypeDef debugInit()
{
    printQueue = xQueueCreate(PRINT_QUEUE_LENGTH, PRINT_QUEUE_STRING_SIZE);
    if (!printQueue)
    {
        return HAL_ERROR;
    }

    uartRxQueue = xQueueCreate(UART_RX_QUEUE_LENGTH, 1);
    if (!printQueue)
    {
        return HAL_ERROR;
    }
	return HAL_OK;
}

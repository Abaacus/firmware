#include "FreeRTOS.h"
#include "task.h"

#include "bsp.h"
#include "debug.h"
#include "controlStateMachine.h"
#include "userCan.h"
#include "controlStateMachine_mock.h"
#include "LTC4110.h"
#include "errorHandler.h"

void vApplicationStackOverflowHook( TaskHandle_t xTask,
                                    signed char *pcTaskName )
{
    HAL_GPIO_WritePin(ERROR_LED_PORT, ERROR_LED_PIN, GPIO_PIN_SET);
    printf("Stack overflow for task %s\n", pcTaskName);
}


// This is declared with weak linkage in all Cube main.c files, and called
// before freeRTOS initializes and starts up
void userInit()
{
    /* Should be the first thing initialized, otherwise print will fail */
    if (debugInit() != HAL_OK) {
        PDU_error(Failure_debugInit);
    }

    if (initStateMachines() != HAL_OK) {
        ERROR_PRINT("Failed to init state machines!\n");
        PDU_error(Failure_initStateMachines);
    }

    if (canInit(&CAN_HANDLE) != HAL_OK) {
      PDU_error(Failure_canInit);
    }

    if (mockStateMachineInit() != HAL_OK) {
        ERROR_PRINT("Failed to init state machines!\n");
        PDU_error(Failure_mockStateMachineInit);
    }

    uartStartReceiving(&DEBUG_UART_HANDLE);
}


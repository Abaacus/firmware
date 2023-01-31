#include "cooling.h"
#include "bsp.h"
#include "FreeRTOS.h"
#include "task.h"
#include "state_machine.h"
#include "controlStateMachine.h"
#include "FreeRTOS_CLI.h"
#include "debug.h"
#include "adc.h"
#include <stdbool.h>
#include "pdu_can.h"
#include "watchdog.h"
#include "canReceive.h"
#include "pdu_dtc.h"

void coolingControlTask(void *pvParameters)
{
     if (registerTaskToWatch(COOLING_TASK_ID, COOLING_TASK_INTERVAL_MS, false, NULL) != HAL_OK)
    {
        ERROR_PRINT("Failed to register cooling task with watchdog!\n");
        Error_Handler();
    }

    // Delay to allow system to turn on
    vTaskDelay(100);

    TickType_t xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
        uint16_t tempMRight = TempMotorRight;
        uint16_t tempMLeft = TempMotorLeft;

        if(tempMRight>COOLING_THRESHOLD || tempMLeft>COOLING_THRESHOLD){
            if(fsmGetState(&coolingFsmHandle) != COOL_STATE_ON){
                fsmSendEvent(&coolingFsmHandle, COOL_EV_OVERTEMP_WARNING, portMAX_DELAY);
            }
        }
        else{
            if(fsmGetState(&coolingFsmHandle) == COOL_STATE_ON){
                fsmSendEvent(&coolingFsmHandle, COOL_EV_EM_DISABLE, portMAX_DELAY);
            }
        }

        watchdogTaskCheckIn(COOLING_TASK_ID);
        vTaskDelayUntil(&xLastWakeTime, COOLING_TASK_INTERVAL_MS);
    }


}

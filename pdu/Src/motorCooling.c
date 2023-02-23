#include <stdbool.h>
#include "motorCooling.h"
#include "bsp.h"
#include "FreeRTOS.h"
#include "task.h"
#include "state_machine.h"
#include "controlStateMachine.h"
#include "FreeRTOS_CLI.h"
#include "debug.h"
#include "adc.h"
#include "pdu_can.h"
#include "watchdog.h"
#include "canReceive.h"
#include "pdu_dtc.h"

void motorCoolingTask(void *pvParameters)
{
     if (registerTaskToWatch(COOLING_TASK_ID, 2*COOLING_TASK_INTERVAL_MS, false, NULL) != HAL_OK)
    {
        ERROR_PRINT("Failed to register cooling task with watchdog!\n");
        Error_Handler();
    }

    //intializing variables to 0 to avoid unexpected behaviour until inverters are turned on
    TempMotorLeft = 0;
    TempMotorRight = 0;

    // Delay to allow system to turn on
    vTaskDelay(100);

    TickType_t xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
        uint16_t tempMRight = averageTempRight;
        uint16_t tempMLeft = averageTempLeft;

        if( ( tempMRight >= COOLING_THRESHOLD || tempMLeft >= COOLING_THRESHOLD ) && fsmGetState(&coolingFsmHandle) != COOL_STATE_ON ){
                fsmSendEvent(&coolingFsmHandle, COOL_EV_OVERTEMP_WARNING, portMAX_DELAY);
        }
        else if( ( tempMRight <= (COOLING_THRESHOLD - HYSTERESIS) || tempMLeft <= (COOLING_THRESHOLD - HYSTERESIS) ) && fsmGetState(&coolingFsmHandle) == COOL_STATE_ON ) {
                fsmSendEvent(&coolingFsmHandle, COOL_EV_EM_DISABLE, portMAX_DELAY);
        }

        tempMotorRightSum = 0;
        rightCount = 0;
        averageTempRight = 0;

        tempMotorLeftSum = 0;
        leftCount = 0;
        averageTempLeft = 0;

        watchdogTaskCheckIn(COOLING_TASK_ID);
        vTaskDelayUntil(&xLastWakeTime, COOLING_TASK_INTERVAL_MS);
    }
}

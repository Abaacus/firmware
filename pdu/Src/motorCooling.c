#include <stdbool.h>
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
#include "motorCooling.h"

float tempMotorRightSum;
uint32_t numTempSamplesRight;
float averageTempRight;

float tempMotorLeftSum;
uint32_t numTempSamplesLeft;
float averageTempLeft;

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

    TickType_t xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
        const bool motorOverTemp = averageTempLeft >= COOLING_START_THRESHOLD_C|| averageTempRight >= COOLING_START_THRESHOLD_C;

        switch (fsmGetState(&coolingFsmHandle))
        {
            case COOL_STATE_ON:
                if( !motorOverTemp ){
                    fsmSendEvent(&coolingFsmHandle, COOL_EV_DISABLE, portMAX_DELAY);
                }
                break;
            case COOL_STATE_OFF:
                if( motorOverTemp && 
                    fsmGetState(&motorFsmHandle) == MTR_STATE_Motors_On ){
                    fsmSendEvent(&coolingFsmHandle, COOL_EV_OVERTEMP_WARNING, portMAX_DELAY);
                }
                break;
            case COOL_STATE_WAIT:
                break;
            case COOL_STATE_LV_Cuttoff:
                break;
            case COOL_STATE_HV_CRITICAL:
                break;

            default:
                //do nothing
                break;
        }

        //Reset all values
        tempMotorRightSum = 0;
        numTempSamplesRight = 0;
        averageTempRight = 0;

        tempMotorLeftSum = 0;
        numTempSamplesLeft = 0;
        averageTempLeft = 0;

        watchdogTaskCheckIn(COOLING_TASK_ID);
        vTaskDelayUntil(&xLastWakeTime, COOLING_TASK_INTERVAL_MS);
    }
}

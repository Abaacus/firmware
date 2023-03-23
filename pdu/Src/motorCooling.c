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

float tempMotorRightSum = 0;
uint32_t numMotorTempSamplesRight = 0;
float averageTempMotorRight = 0;

float tempMotorLeftSum = 0;
uint32_t numMotorTempSamplesLeft = 0;
float averageTempMotorLeft = 0;

void coolingOff(void) {
    DEBUG_PRINT("Turning cooling off\n");
    PUMP_LEFT_DISABLE;
    PUMP_RIGHT_DISABLE;
    FAN_LEFT_DISABLE;
    FAN_RIGHT_DISABLE;
}

void coolingOn(void) {
    DEBUG_PRINT("Turning cooling on\n");
    PUMP_LEFT_ENABLE;
    PUMP_RIGHT_ENABLE;
    FAN_LEFT_ENABLE;
    FAN_RIGHT_ENABLE;
}

void motorCoolingTask(void *pvParameters)
{
     if (registerTaskToWatch(COOLING_TASK_ID, 2*COOLING_TASK_INTERVAL_MS, false, NULL) != HAL_OK)
    {
        ERROR_PRINT("Failed to register cooling task with watchdog!\n");
        Error_Handler();
    }

    static uint32_t cooling_state = COOL_STATE_OFF;
    //intializing variables to 0 to avoid unexpected behaviour until inverters are turned on
    TempMotorLeft = 0;
    TempMotorRight = 0;

    TickType_t xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
         if(numMotorTempSamplesRight != 0){
            averageTempMotorRight = tempMotorRightSum/numMotorTempSamplesRight;
        }

        if(numMotorTempSamplesLeft != 0){
            averageTempMotorLeft = tempMotorLeftSum/numMotorTempSamplesLeft;
        }

        const bool motorOverTemp = (averageTempMotorLeft >= COOLING_START_THRESHOLD_C) || (averageTempMotorRight >= COOLING_START_THRESHOLD_C);

        switch (cooling_state)
        {
            case COOL_STATE_ON:
                if( averageTempMotorLeft <= COOLING_STOP_THRESHOLD_C && averageTempMotorRight <= COOLING_STOP_THRESHOLD_C )
                {
                    coolingOff();
                    cooling_state = COOL_STATE_OFF;
                }
                break;
            case COOL_STATE_OFF:
                if( (motorOverTemp) && 
                    (fsmGetState(&motorFsmHandle) == MTR_STATE_EM_Enable) )
                {
                    coolingOn();
                    cooling_state = COOL_STATE_ON;
                }
                break;
            case COOL_STATE_WAIT:
            case COOL_STATE_LV_Cuttoff:
            case COOL_STATE_HV_CRITICAL:
            default:
                //do nothing
                break;
        }

        //Reset all values
        tempMotorRightSum = 0;
        numMotorTempSamplesRight = 0;
        averageTempMotorRight = 0;

        tempMotorLeftSum = 0;
        numMotorTempSamplesLeft = 0;
        averageTempMotorLeft = 0;

        watchdogTaskCheckIn(COOLING_TASK_ID);
        vTaskDelayUntil(&xLastWakeTime, COOLING_TASK_INTERVAL_MS);
    }
}

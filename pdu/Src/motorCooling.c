#include <stdbool.h>
#include "bsp.h"
#include "FreeRTOS.h"
#include "task.h"
#include "state_machine.h"
#include "controlStateMachine.h"
#include "FreeRTOS_CLI.h"
#include "timers.h"
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

static uint8_t activeEventsMask = 0;

void coolingSystemOff(void) 
{
    DEBUG_PRINT("Turning cooling off\n");
    PUMP_LEFT_DISABLE;
    PUMP_RIGHT_DISABLE;
    FAN_LEFT_DISABLE;
    FAN_RIGHT_DISABLE;
}

void coolingSystemOn(void) 
{
    DEBUG_PRINT("Turning cooling on\n");
    PUMP_LEFT_ENABLE;
    PUMP_RIGHT_ENABLE;
    FAN_LEFT_ENABLE;
    FAN_RIGHT_ENABLE;
}

void sendCoolingEvent(coolingEvents event)
{
    activeEventsMask |= (1 << event);
}

void motorCoolingTask(void *pvParameters)
{
    if (registerTaskToWatch(COOLING_TASK_ID, 2*COOLING_TASK_INTERVAL_MS, false, NULL) != HAL_OK)
    {
        ERROR_PRINT("Failed to register cooling task with watchdog!\n");
        Error_Handler();
    }

    TempMotorLeft = 0;
    TempMotorRight = 0;

    static uint32_t cooling_state = COOL_STATE_WAIT;

    TickType_t xLastWakeTime = xTaskGetTickCount();
    TickType_t xCoolingTickCount = 0;

    while(1)
    {
        if (numMotorTempSamplesRight != 0)
        {
            averageTempMotorRight = tempMotorRightSum/numMotorTempSamplesRight;
        }

        if (numMotorTempSamplesLeft != 0)
        {
            averageTempMotorLeft = tempMotorLeftSum/numMotorTempSamplesLeft;
        }

        const bool motorOverTemp = (averageTempMotorLeft >= COOLING_START_THRESHOLD_C) || 
                                    (averageTempMotorRight >= COOLING_START_THRESHOLD_C);

        switch (cooling_state)
        {
            case COOL_STATE_ON:
                if (activeEventsMask & (1 << COOLING_EVENT_FATAL_DTC))
                {
                    ERROR_PRINT("Fatal event received, shutting cooling.");
                    coolingSystemOff();
                    cooling_state = COOL_STATE_HV_CRITICAL;
                    activeEventsMask = 0;
                }
                else if (activeEventsMask & (1 << COOLING_EVENT_LV_CUTOFF))
                {
                    ERROR_PRINT("LV cutofff event received, shutting cooling.");
                    coolingSystemOff();
                    cooling_state = COOL_STATE_LV_Cuttoff;
                    activeEventsMask = 0;
                }
                else if (activeEventsMask & (1 << COOLING_EVENT_DISABLE))
                {
                    coolingSystemOff();
                    cooling_state = COOL_STATE_OFF;
                    activeEventsMask = 0;
                }
                else if ((averageTempMotorLeft <= COOLING_STOP_THRESHOLD_C) && 
                        (averageTempMotorRight <= COOLING_STOP_THRESHOLD_C))
                {
                    coolingSystemOff();
                    cooling_state = COOL_STATE_OFF;
                }
                break;
            case COOL_STATE_OFF:
                if (activeEventsMask & (1 << COOLING_EVENT_FATAL_DTC))
                {
                    ERROR_PRINT("Fatal event received, shutting cooling.");
                    coolingSystemOff();
                    cooling_state = COOL_STATE_HV_CRITICAL;
                    activeEventsMask = 0;
                }
                else if (activeEventsMask & (1 << COOLING_EVENT_LV_CUTOFF))
                {
                    ERROR_PRINT("LV cutofff event received, shutting cooling.");
                    coolingSystemOff();
                    cooling_state = COOL_STATE_LV_Cuttoff;
                    activeEventsMask = 0;
                }
                else if ((motorOverTemp) && 
                        (fsmGetState(&motorFsmHandle) == MTR_STATE_EM_Enable))
                {
                    coolingSystemOn();
                    cooling_state = COOL_STATE_ON;
                }
                break;
            case COOL_STATE_WAIT:
                xCoolingTickCount = xTaskGetTickCount();

                if (activeEventsMask & (1 << COOLING_EVENT_FATAL_DTC))
                {
                    ERROR_PRINT("Fatal event received, shutting cooling.");
                    coolingSystemOff();
                    cooling_state = COOL_STATE_HV_CRITICAL;
                    activeEventsMask = 0;
                }
                else if (activeEventsMask & (1 << COOLING_EVENT_LV_CUTOFF))
                {
                    ERROR_PRINT("LV cutofff event received, shutting cooling.");
                    coolingSystemOff();
                    cooling_state = COOL_STATE_LV_Cuttoff;
                    activeEventsMask = 0;
                }
                else if (xTaskGetTickCount() >= (xCoolingTickCount + 1000))
                {
                    sendCoolingEvent(COOLING_EVENT_DELAY_ELAPSED);
                    cooling_state = COOL_STATE_OFF;
                    activeEventsMask = 0;
                }
                break;
            case COOL_STATE_LV_Cuttoff:
                cooling_state = COOL_STATE_LV_Cuttoff;
                ERROR_PRINT("Low Voltage Cut Off");
                break;
            case COOL_STATE_HV_CRITICAL:
                cooling_state = COOL_STATE_HV_CRITICAL;
                ERROR_PRINT("Cooling System Critical");
                break;
            default:
                coolingSystemOff();
                cooling_state = COOL_STATE_HV_CRITICAL;
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

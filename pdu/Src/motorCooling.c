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

uint8_t eventPDU = DEFAULT_EV;
static uint32_t cooling_state = COOL_STATE_WAIT;

TimerHandle_t coolingSystemDelayTimer;
TimerHandle_t fatalFailureDelayTimer;
TimerHandle_t LVCuttoffDelayTimer;

void coolingSystemDelayCallback(TimerHandle_t timer)
{
    eventPDU = COOLING_DELAY_TIMER_ELAPSED;
}

void fatalFailureDelayCallback(TimerHandle_t timer)
{
    coolingSystemOff();
    setCoolingState(COOL_STATE_HV_CRITICAL);
    eventPDU = DEFAULT_EV;
}

void LVCuttoffDelayCallback(TimerHandle_t timer)
{
    coolingSystemOff();
    setCoolingState(COOL_STATE_LV_Cuttoff);
    eventPDU = DEFAULT_EV;
}

void timerInit(void)
{
    coolingSystemDelayTimer = xTimerCreate("COOLING_DELAY",
                                            pdMS_TO_TICKS(COOLING_TIMER_DELAY_MS),
                                            pdFALSE /* Auto Reload */,
                                            0,
                                            coolingSystemDelayCallback);
    
    fatalFailureDelayTimer = xTimerCreate("FATAL_FAILURE_DELAY_TIMER",
                                            pdMS_TO_TICKS(FATAL_FAILURE_DELAY_MS),
                                            pdFALSE /* Auto Reload */,
                                            0,
                                            fatalFailureDelayCallback);
    
    LVCuttoffDelayTimer = xTimerCreate("LV_CUTTOFF_DELAY_TIMER",
                                            pdMS_TO_TICKS(LV_CUTTOFF_DELAY_MS),
                                            pdFALSE /* Auto Reload */,
                                            0,
                                            LVCuttoffDelayCallback);
    
}

void setCoolingState(uint32_t newState)
{
    //Should not exit COOL_STATE_HV_CRITICAL or COOL_STATE_LV_Cuttoff
    if ((getCoolingState() != COOL_STATE_HV_CRITICAL) && (getCoolingState() != COOL_STATE_LV_Cuttoff))
    {
        cooling_state = newState;
    }
}

uint32_t getCoolingState(void)
{
    return cooling_state;
}

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

void motorCoolingTask(void *pvParameters)
{
    if (registerTaskToWatch(COOLING_TASK_ID, 2*COOLING_TASK_INTERVAL_MS, false, NULL) != HAL_OK)
    {
        ERROR_PRINT("Failed to register cooling task with watchdog!\n");
        Error_Handler();
    }
    
    timerInit();

    TempMotorLeft = 0;
    TempMotorRight = 0;

    TickType_t xLastWakeTime = xTaskGetTickCount();
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

        if (eventPDU == FATAL_DTC_EV)
        {
            ERROR_PRINT("fatal DTC event received: turning off cooling.");
            if (xTimerStart(fatalFailureDelayTimer, 100) != pdPASS)
            {
                ERROR_PRINT("Failed to start fatal failure timer, turning off cooling.");
                coolingSystemOff();
                setCoolingState(COOL_STATE_HV_CRITICAL);
            }
        }
        else if (eventPDU == LV_CUTTOFF_EV)
        {
            ERROR_PRINT("LV cuttoff event received: turning off cooling.");
            if (xTimerStart(LVCuttoffDelayTimer, 100) != pdPASS)
            {
                ERROR_PRINT("Failed to start LV cuttoff timer, turning off cooling.");
                coolingSystemOff();
                setCoolingState(COOL_STATE_HV_CRITICAL);
            }
        }
        else if (eventPDU == COOLING_DELAY_TIMER_ELAPSED)
        {
            setCoolingState(COOL_STATE_OFF);
            eventPDU = DEFAULT_EV;
        }

        switch (getCoolingState())
        {
            case COOL_STATE_ON:
                if ((averageTempMotorLeft <= COOLING_STOP_THRESHOLD_C) && 
                    (averageTempMotorRight <= COOLING_STOP_THRESHOLD_C))
                {
                    coolingSystemOff();
                    setCoolingState(COOL_STATE_OFF);
                }
                break;
            case COOL_STATE_OFF:
                if ((motorOverTemp) && 
                    (fsmGetState(&motorFsmHandle) == MTR_STATE_EM_Enable))
                {
                    coolingSystemOn();
                    setCoolingState(COOL_STATE_ON);
                }
                break;
            case COOL_STATE_WAIT:
                if (xTimerStart(coolingSystemDelayTimer, 100)!= pdPASS) 
                {
                    ERROR_PRINT("Failed to start cooling system delay timer");
                    setCoolingState(COOL_STATE_OFF);
                }
                break;
            case COOL_STATE_LV_Cuttoff:
                setCoolingState(COOL_STATE_LV_Cuttoff);
                ERROR_PRINT("Low Voltage Cut Off");
                break;
            case COOL_STATE_HV_CRITICAL:
                setCoolingState(COOL_STATE_HV_CRITICAL);
                ERROR_PRINT("Cooling System Critical");
                break;
            default:
                coolingSystemOff();
                setCoolingState(COOL_STATE_HV_CRITICAL);
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

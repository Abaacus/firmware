/**
  *****************************************************************************
  * @file    fanControl.c
  * @author  Richard Matthews
  * @brief   Module controlling battery pack fans.
  * @details The battery pack contains an array of fans to cool the battery.
  * The BMU controls the fans based on the temperature of the battery cells,
  * turning them on as the cells heat up and increasing the fan speed
  * proportional to temeprature.
  ******************************************************************************
  */

#include "bsp.h"
#include "fanControl.h"
#include "bmu_can.h"
#include "batteries.h"
#include "debug.h"
#include "controlStateMachine.h"
#include "state_machine.h"

#define FAN_OFF_TEMP 30
// Fans need pwm of 25 kHz, so we set timer to have 10 MHz freq, and 400 period
#define FAN_MAX_DUTY_PERCENT 1.0
#define FAN_ON_DUTY_PERCENT 0.2
#define FAN_PERIOD_COUNT 400
#define FAN_TASK_PERIOD_MS 100

uint32_t calculateFanPeriod()
{
  // PWM Output is inverted from what we generate from PROC

  // Full fan while charging
  if (fsmGetState(&fsmHandle) == STATE_Charging) {
    /*DEBUG_PRINT("Charging fans\n");*/
    return FAN_PERIOD_COUNT - FAN_PERIOD_COUNT*FAN_MAX_DUTY_PERCENT;
  }

  if (TempCellMax < FAN_OFF_TEMP) {
    return FAN_PERIOD_COUNT;
  }

  return FAN_PERIOD_COUNT
    - map_range_float(TempCellMax, FAN_OFF_TEMP, CELL_MAX_TEMP_C,
                      FAN_PERIOD_COUNT*FAN_ON_DUTY_PERCENT,
                      FAN_PERIOD_COUNT*FAN_MAX_DUTY_PERCENT);
}

HAL_StatusTypeDef fanInit()
{
  __HAL_TIM_SET_COMPARE(&FAN_HANDLE, TIM_CHANNEL_1, 0);

  if (HAL_TIM_PWM_Start(&FAN_HANDLE, TIM_CHANNEL_1) != HAL_OK)
  {
     ERROR_PRINT("Failed to start fan pwm timer\n");
     return HAL_ERROR;
  }

  return HAL_OK;
}

HAL_StatusTypeDef setFan()
{
   int duty = calculateFanPeriod();

    __HAL_TIM_SET_COMPARE(&FAN_HANDLE, TIM_CHANNEL_1, duty);

    return HAL_OK;
}

/**
 * Task to control the battery box fans.
 */
void fanTask()
{
  if (fanInit() != HAL_OK) {
    ERROR_PRINT("Failed to init fans\n");
    Error_Handler();
  }

  while (1) {
    setFan();
    vTaskDelay(pdMS_TO_TICKS(FAN_TASK_PERIOD_MS));
  }
}

/**** Measuring Fan RPM ****/
#define TIMCLOCK   16000000 //16 MHz
#define PRESCALAR  16

/* Captured Value */
volatile uint32_t IC_Val1 = 0;
volatile uint32_t IC_Val2 = 0;
uint32_t Difference = 0;
int Is_First_Captured = 0;

float frequency = 0;
uint32_t RPM = 0;

HAL_StatusTypeDef begin_fanRPM_measurement(void) {
  /*Start the Input Capture in interrupt mode*/
  if (HAL_TIM_IC_Start_IT(&FAN_TIM_HANDLE, TIM_CHANNEL_1) != HAL_OK)
  {
    /* Starting Error */
    print_error("error starting status timer");
    return HAL_ERROR;
  }
  //Timer 4 needs to be added here as well
  return HAL_OK;
}

HAL_StatusTypeDef stop_fanRPM_measurement(void) {
  if (HAL_TIM_IC_Stop_IT(&FAN_TIM_HANDLE, TIM_CHANNEL_1) != HAL_OK) {
    print_error("Failed to stop timer\n");
    return HAL_ERROR;
  }

  return HAL_OK;
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
	{
		if (Is_First_Captured==0) // if the first rising edge is not captured
		{
			IC_Val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read the first value
			Is_First_Captured = 1;  // set the first captured as true
		}

		else   // If the first rising edge is captured, now we will capture the second edge
		{
			IC_Val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);  // read second value

			if (IC_Val2 > IC_Val1)
			{
				Difference = IC_Val2-IC_Val1; 
			}

			else if (IC_Val1 > IC_Val2) //overflow situation
			{
				Difference = (0xffff - IC_Val1) + IC_Val2;
			}

			float refClock = TIMCLOCK/(PRESCALAR);

			frequency = refClock/Difference;

			__HAL_TIM_SET_COUNTER(htim, 0);  // reset the counter
			Is_First_Captured = 0; // set it back to false
      RPM = (frequency/2.0) * 60.0; //fan produces two pulses per revolution
		}
	}
}

uint32_t measure_fanRPM ()
{
  if (begin_fanRPM_measurement() != HAL_OK) {
      ERROR_PRINT("Failed to start fanRPM measurement\n");
      Error_Handler();
   }
  return RPM; 
}

/*
TickType_t xLastWakeTime = xTaskGetTickCount();

void get_fanRPM ()
{
  while (1)
  {
      CONSOLE_PRINT("&d", measure_fanRPM);
      vTaskDelayUntil(&xLastWakeTime, FAN_TASK_PERIOD_MS);
  }
}
*/
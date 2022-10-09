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

/**** Measuring Average RPM of 5 Fans ****/
#define ONE_SEC_MS 1000

/* Captured Value */
uint32_t FAN_RPM = 0;
uint32_t SIGNAL_COUNTER = 0;

void begin_fanRPM_measurement()
{
  HAL_Init();
 
  SystemClock_Config();
 
  MX_GPIO_Init();
 
  while (1)
  {
    FAN_RPM = SIGNAL_COUNTER/(60*2*5); //2 signals per revolution and 5 fans
    vTaskDelay(ONE_SEC_MS);
  }
}
 
// EXTI External Interrupt ISR Handler CallBackFun
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == GPIO_PIN_6 || GPIO_PIN_12 || GPIO_PIN_13 || GPIO_PIN_14 || GPIO_PIN_15)
    {
      SIGNAL_COUNTER++; // increment counter
    }
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
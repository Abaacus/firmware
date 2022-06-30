/**
  *****************************************************************************
  * @file    fanMonitor.c
  * @author  Jacky Lim
  * @brief   Module monitoring fan speed
  * @details The battery pack has 5 fans to cool the battery. 
  * This module calculates the fan speed in RPM based on the fan period.
  ******************************************************************************
  */

#include "bsp.h"
#include "bmu_can.h"
#include "debug.h"
#include "controlStateMachine.h"
#include "state_machine.h"
#include "FreeRTOS_CLI.h"
#include "fanControl.h"

uint32_t calculateFanRPM()
{
    int RPM = 0; // RPM = (1/period) * 60 * 1000
    //if fan is off, return 0. 
    RPM = (1/calculateFanPeriod()) * 60 * 1000;
    //RPM = (FAN_PERIOD_COUNT - __HAL_TIM_GET_COMPARE(&FAN_HANDLE, TIM_CHANNEL_1)) * 60 * 1000 / FAN_TASK_PERIOD_MS;

    return RPM;
}
//consider moving this to fanControl
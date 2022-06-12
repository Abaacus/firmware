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
#include "fanControl.h"

uint32_t calculateFanRPM()
{
    int RPM = 0; // RPM = (1/period) * 60 * 1000
    //if fan is off, return 0. 
    RPM = (1/calculateFanPeriod()) * 60 * 1000;
    //RPM = (FAN_PERIOD_COUNT - __HAL_TIM_GET_COMPARE(&FAN_HANDLE, TIM_CHANNEL_1)) * 60 * 1000 / FAN_TASK_PERIOD_MS;

    return RPM;
}

BaseType_t getFanRPM(char *writeBuffer, size_t writeBufferLength,
                       const char *commandString)
{
    COMMAND_OUTPUT("Fan RPM: %f %%, (adcVal: %lu)\n", (calculateFanRPM));
    //adcVal???

    return pdFALSE;
}

static const CLI_Command_Definition_t getFanRPMDefinition =
{
    "getFan", /* The command string to type. */
    "getFan:\r\n Get Fan RPM\r\n", /* Descriptive help text displayed when the command is typed wrong. */
    getFanRPM, /* The function to run. */
    0 /* Number of parameters */
};

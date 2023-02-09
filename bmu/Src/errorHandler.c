/**
  *****************************************************************************
  * @file    errorHandler.c
  * @author  Richard Matthews
  * @brief   Contains assert failure logging function
  *****************************************************************************
  */

#include "errorHandler.h"
#include "main.h"
#include "FreeRTOS.h"
#include "stdio.h"
#include "stm32f7xx_hal.h"
#include "debug.h"
#include "bmu_dtc.h"

int log_assert_violation(char *file, int line, char *condition)
{
    ERROR_PRINT("ASSERT FAILURE: (%s): %s:%d\n", condition, file, line);
    return 1;
}

void BMU_error(uint8_t error_num)
{
    ERROR_PRINT("BMU ERROR: %u\r\n", error_num);
    sendDTC_CRITICAL_BMU_ERROR_HANDLER(error_num);
}
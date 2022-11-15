#include "errorHandler.h"
#include "main.h"
#include "FreeRTOS.h"
#include "stdio.h"
#include "stm32f7xx_hal.h"
#include "vcu_F7_dtc.h"
#include "bsp.h"
#include "debug.h"

void VCU_error(uint8_t error_num)
{
    ERROR_PRINT("VCU ERROR: %u\r\n", error_num);
    sendDTC_CRITICAL_VCU_ERROR_HANDLER(error_num);
}
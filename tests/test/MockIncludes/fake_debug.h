#ifndef _MOCK_DEBUG_H
#define _MOCK_DEBUG_H
#include "FreeRTOS.h"
#include "stm32f0xx.h"
//#include "fake_hal_defs.h"

// Function Prototypes
HAL_StatusTypeDef fake_debugInit();

void fake_mock_init_debug();
#endif

#ifndef _MY_MOCK_DEBUG_H
#define _MY_MOCK_DEBUG_H
#include "FreeRTOS.h"
#ifdef BOARD_TYPE_F0
#include "stm32f0xx.h"
#else
#ifdef BOARD_TYPE_F7
#include "stm32f7xx.h"
#else
#error "Incorrect board used for unit testing, select F7 or F0, Nucleo target is not supported"
#endif
#endif
//#include "fake_hal_defs.h"

// Function Prototypes
HAL_StatusTypeDef fake_debugInit();

void fake_mock_init_debug(void);
#endif

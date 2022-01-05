#ifndef FAKE_CAN_H
#define FAKE_CAN_H
#include "FreeRTOS.h"

typedef void* CAN_HandleTypeDef;
typedef void* CAN_FilterTypeDef;

#if BOARD_TYPE_F0
extern CAN_HandleTypeDef hcan;
#elif BOARD_TYPE_F7
extern CAN_HandleTypeDef hcan3;
#endif

HAL_StatusTypeDef fake_HAL_CAN_ConfigFilter(CAN_HandleTypeDef *hcan, CAN_FilterTypeDef *sFilterConfig);

#endif

#ifndef FAKE_CAN_H
#define FAKE_CAN_H
#include "userCan.h"

extern CAN_HandleTypeDef hcan;

HAL_StatusTypeDef HAL_CAN_ConfigFilter(CAN_HandleTypeDef *hcan, CAN_FilterTypeDef *sFilterConfig);

#endif

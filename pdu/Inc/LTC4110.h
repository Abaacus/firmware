#ifndef LTC4110_H
#define LTC4110_H
#include "stm32f7xx_hal.h"
#include "stdbool.h"

#define POWER_TASK_INTERVAL_MS 250

void powerTask(void *pvParameters);

HAL_StatusTypeDef LTC4110Init(void);

#endif /* end of include guard: SENSORS_H */
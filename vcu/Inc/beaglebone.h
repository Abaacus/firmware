#ifndef BEAGLEBONE_H

#define BEAGLEBONE_H

#include "bsp.h"

HAL_StatusTypeDef beaglebonePower(bool enable);
HAL_StatusTypeDef beagleboneOff();
void bbTask(void *pvParameters);

#endif /* end of include guard: BEAGLEBONE_H */

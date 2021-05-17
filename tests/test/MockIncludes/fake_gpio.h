#ifndef FAKE_GPIO_H
#define FAKE_GPIO_H

#include <stdint.h>
#include "fake_hal_defs.h"



void HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);

#endif

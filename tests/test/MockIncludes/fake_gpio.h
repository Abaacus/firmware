#ifndef FAKE_GPIO_H
#define FAKE_GPIO_H

#include <stdint.h>
#include "fake_hal_defs.h"
#include "gpio.h"


void HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

#endif

#ifndef ADE7913_H_
#define ADE7913_H_

#include "stm32f0xx_hal.h"
HAL_StatusTypeDef cell_adc_init();
HAL_StatusTypeDef adc_read(uint8_t addr, int32_t *dataOut);
HAL_StatusTypeDef adc_readbyte(uint8_t addr, uint8_t *dataOut);
HAL_StatusTypeDef adc_write(uint8_t addr, uint8_t data);
HAL_StatusTypeDef adc_read_current(float *dataOut);
HAL_StatusTypeDef adc_read_v1(float *dataOut);
HAL_StatusTypeDef adc_read_v2(float *dataOut);

#endif /* ADE7013_H_*/

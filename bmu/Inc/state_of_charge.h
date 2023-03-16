#ifndef STATE_OF_CHARGE_H
#define STATE_OF_CHARGE_H
#include "FreeRTOS.h"
#include "stm32f7xx_hal.h"

void integrate_bus_current(float IBus, float period_ms);
HAL_StatusTypeDef socInit(void);
float getIBusIntegrated(); //getter setter functions for Ibus_integrated called by controlStateMachine_mock.c
float setIBusIntegrated(float IBus_integrated_input);


#endif

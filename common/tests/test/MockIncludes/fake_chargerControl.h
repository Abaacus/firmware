#ifndef FAKE_CHARGER_CONTROL_H
#define FAKE_CHARGER_CONTROL_H
#include "FreeRTOS.h"
#include "can_interfaces.h"
#include "bmu_can.h"

HAL_StatusTypeDef chargerInit();

#endif
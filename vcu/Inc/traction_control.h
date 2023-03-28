#ifndef TRACTION_CONTROL_H
#define TRACTION_CONTROL_H
#include "stdbool.h"
#include "bsp.h"

void toggle_TC(void);
void disable_TC(void);
void set_TC(bool tc_bool);
HAL_StatusTypeDef set_tc_kP(float tc_kP_value);
HAL_StatusTypeDef set_error_floor(float error_floor_value);
HAL_StatusTypeDef set_adjustment_torque_floor(float adjustment_torque_floor_value);
#endif

#ifndef TRACTION_CONTROL_H
#define TRACTION_CONTROL_H
#include "stdbool.h"
#include "bsp.h"

void toggle_TC(void);
void disable_TC(void);
void set_TC_enabled(bool tc_bool);
HAL_StatusTypeDef set_tc_kP(float tc_kP_value);
HAL_StatusTypeDef set_tc_error_floor(float error_floor_value);
HAL_StatusTypeDef set_adjustment_torque_floor(float adjustment_torque_floor_value);
float get_tc_kP(void);
float get_error_floor(void);
float get_adjustment_torque_floor(void);
bool get_TC(void);
#endif

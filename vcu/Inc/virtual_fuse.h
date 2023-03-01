#ifndef VIRTUAL_FUSE_H
#define VIRTUAL_FUSE_H

#include "vcu_F7_can.h"
#include "brakeAndThrottle.h"
#include "motorController.h"
#include "debug.h"

float get_cooling_factor();
void set_cooling_factor(float new_cooling_factor);

float get_linear_cutoff();
void set_linear_cutoff(float new_linear_cutoff);

float get_normal_cutoff();
void set_normal_cutoff(float new_normal_cutoff);

float get_motor_power_safe();
void set_motor_power_safe(float new_motor_power_safe);

float get_motor_linear_factor();
void set_motor_linear_factor(float new_motor_linear_factor);

HAL_StatusTypeDef check_virtual_fuse(float i2t, float t_batt, float t_amb);

#endif

#ifndef FANCONTROL_H
#define FANCONTROL_H

#include <stdbool.h>

#define FAN_OFF_TEMP 25
#define FAN_PEAK_TEMP 35
// Fans need pwm of 25 kHz, so we set timer to have 10 MHz freq, and 400 period
#define FAN_MAX_DUTY_PERCENT 1.0
#define FAN_ON_DUTY_PERCENT 0.2
#define FAN_PERIOD_COUNT 400
#define FAN_TASK_PERIOD_MS 1000

bool overrideFanControl = false;
float fanOverridePercent = 0.;

#endif /* end of include guard: FANCONTROL_H */

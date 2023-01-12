#ifndef TRACTION_CONTROL_H
#define TRACTION_CONTROL_H
#include <stdint.h>

extern uint16_t traction_control_task_period;

void toggle_TC(void);
void disable_TC(void);
#endif

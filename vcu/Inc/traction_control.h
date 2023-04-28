#ifndef TRACTION_CONTROL_H
#define TRACTION_CONTROL_H

void toggle_TC(void);
void disable_TC(void);
extern float tc_kP, tc_kI, tc_kD, desired_slip;
#endif

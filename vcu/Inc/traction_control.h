#ifndef TRACTION_CONTROL_H
#define TRACTION_CONTROL_H

extern float tc_kP;
extern float error_floor_rad_s;
extern float torque_demand_floor;

void toggle_TC(void);
void disable_TC(void);

#endif /* TRACTION_CONTROL_H */
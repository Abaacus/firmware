#ifndef FANCONTROL_H

#define FANCONTROL_H

uint32_t calculateFanRPM();
uint32_t calculateFanPeriod();
void begin_fanRPM_measurement();
void sendFanRPM();

#endif /* end of include guard: FANCONTROL_H */

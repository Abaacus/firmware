#ifndef BRAKELIGHT_H

#define BRAKELIGHT_H

#include <stdint.h>
#include "debug.h"

bool isBrakePressed(uint32_t brakePercent);
void CAN_Msg_VCU_Data_Callback();

#endif /* end of include guard: BRAKELIGHT_H */

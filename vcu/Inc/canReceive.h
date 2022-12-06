#ifndef CAN_RECEIVE_H_
#define CAN_RECEIVE_H_

#include "stdbool.h"
#include "bsp.h"

#define MC_ENCODER_OFFSET 32768

bool getHvEnableState();
bool getMotorControllersStatus();
uint32_t lastBrakeValReceiveTimeTicks;

extern volatile int16_t speedMotorLeftRPM;
extern volatile int16_t speedMotorRightRPM;

#endif /* USER_CAN_H_ */

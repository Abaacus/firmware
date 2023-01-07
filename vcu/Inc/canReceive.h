#ifndef CAN_RECEIVE_H_
#define CAN_RECEIVE_H_

#include "stdbool.h"
#include "bsp.h"

#include "canReceiveCommon.h"

void DTC_History_init();
DTC_History_t * get_DTC_History();

bool getHvEnableState();
bool getMotorControllersStatus();
uint32_t lastBrakeValReceiveTimeTicks;

#endif /* USER_CAN_H_ */

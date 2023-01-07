#ifndef CAN_RECEIVE_H_
#define CAN_RECEIVE_H_

#include "bsp.h"
#include "canReceiveCommon.h"

extern uint32_t lastChargeCartHeartbeat;

void DTC_History_init();
DTC_History_t * get_DTC_History();

#endif /* USER_CAN_H_ */

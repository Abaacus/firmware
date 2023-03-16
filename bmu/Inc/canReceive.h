#ifndef CAN_RECEIVE_H_
#define CAN_RECEIVE_H_

#include "bsp.h"

extern uint32_t lastChargeCartHeartbeat;

/* Enum for checking VariableEnumBMU*/
 typedef enum BMU_Variable_Name {
    maxChargeCurrent_name = 0,
    adjustedCellI_name,
    maxChargeVoltage_name,
    stateBusHVSendPeriod_name,
    capacityStartup_name,
    iBusIntegrated_name,
} BMU_Variable_Name;

#endif /* USER_CAN_H_ */

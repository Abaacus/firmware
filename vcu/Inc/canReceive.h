#ifndef CAN_RECEIVE_H_
#define CAN_RECEIVE_H_

#include "stdbool.h"
#include "bsp.h"

bool getHvEnableState();
bool getMotorControllersStatus();
uint32_t lastBrakeValReceiveTimeTicks;
typedef enum VCU_Variable_Name {
    initialSoc_name = 0,
    numLaps_name,
    numLapsToComplete_name,
    inEnduranceMode_name,
    emkP_name,
    emkI_name,
    tcOn_name,
    tckP_name,
    errorFloor_name,
    adjustmentTorqueFloor_name,
    tvDeadzoneEndRight_name,
    tvDeadzoneEndLeft_name,
    torqueVectorFactor_name,
    maxTorqueDemandDefault_name,
} VCU_Variable_Name;

#endif /* USER_CAN_H_ */

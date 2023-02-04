#ifndef CANRECEIVE_H

#define CANRECEIVE_H

#include "bsp.h"

bool getHVState();
bool getEMState();
void CAN_Msg_BMU_HV_Power_State_Callback();
void CAN_Msg_VCU_EM_State_Callback();
void CAN_Msg_BMU_DTC_Callback(int DTC_CODE, int DTC_Severity, int DTC_Data);
void DTC_Fatal_Callback(BoardIDs board);

#endif /* end of include guard: CANRECEIVE_H */


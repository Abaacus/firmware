#ifndef CAN_RECEIVE_H_
#define CAN_RECEIVE_H_

#include "stdbool.h"
#include "bsp.h"

bool getHvEnableState();
bool getMotorControllersStatus(void);
extern uint32_t lastBrakeValReceiveTimeTicks;
extern volatile bool motorControllersStatus;
uint32_t lastBrakeValReceiveTimeTicks;
void CAN_Msg_DCU_buttonEvents_Callback(void);
void CAN_Msg_PDU_ChannelStatus_Callback(void);
void DTC_Fatal_Callback(BoardIDs board);
void CAN_Msg_BMU_HV_Power_State_Callback(void);
void CAN_Msg_BMU_BrakePedalValue_Callback(void);
void CAN_Msg_BMU_DTC_Callback(int DTC_CODE, int DTC_Severity, int DTC_Data);

#endif /* USER_CAN_H_ */

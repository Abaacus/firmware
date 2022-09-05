#ifndef CANRECEIVE_H

#define CANRECEIVE_H

extern uint8_t isUartOverCanEnabled;
void CAN_Msg_UartOverCanConfig_Callback();
void CAN_Msg_VCU_EM_Power_State_Request_Callback();
void DTC_Fatal_Callback(BoardIDs board);
void CAN_Msg_BMU_DTC_Callback(int DTC_CODE, int DTC_Severity, int DTC_DATA);

#endif /* end of include guard: CANRECEIVE_H */

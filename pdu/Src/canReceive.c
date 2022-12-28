#include "bsp.h"
#include "pdu_can.h"
#include "pdu_dtc.h"
#include "state_machine.h"
#include "controlStateMachine.h"
#include "debug.h"
#include "boardTypes.h"
#include "canReceive.h"

DTC_History LatestDTCs = { .count = 0 };

void CAN_Msg_UartOverCanConfig_Callback() {
    isUartOverCanEnabled = UartOverCanConfigSignal & 0x4;
}

void CAN_Msg_VCU_EM_Power_State_Request_Callback() {
    if (EM_Power_State_Request == EM_Power_State_Request_On) {
        fsmSendEventISR(&motorFsmHandle, MTR_EV_EM_ENABLE);
        fsmSendEventISR(&coolingFsmHandle, COOL_EV_EM_ENABLE);
    } else {
        fsmSendEventISR(&motorFsmHandle, MTR_EV_EM_DISABLE);
        fsmSendEventISR(&coolingFsmHandle, COOL_EV_EM_DISABLE);
    }
}

void DTC_Fatal_Callback(BoardIDs board) {
    fsmSendEventUrgentISR(&mainFsmHandle, MN_EV_HV_CriticalFailure);
}

//
// DTC Logging
//

void Log_Fatal_DTC(int DTC_CODE, int DTC_Severity, int DTC_Data) {
    if (DTC_Severity == 1) {
        DTC_Received newDTC = { DTC_CODE, DTC_Severity, DTC_Data };

        if (LatestDTCs.count < DTC_HISTORY_LENGTH) {
            LatestDTCs.dtcs[LatestDTCs.count] = newDTC;
            LatestDTCs.count++;
        } else {
            for (int i = 0; i < DTC_HISTORY_LENGTH - 1; i++) {
                LatestDTCs.dtcs[i] = LatestDTCs.dtcs[i + 1];
            }
            LatestDTCs.dtcs[DTC_HISTORY_LENGTH - 1] = newDTC;
        }
    }    
}

void CAN_Msg_DCU_DTC_Callback(int DTC_CODE, int DTC_Severity, int DTC_Data) {
    Log_Fatal_DTC(DTC_CODE, DTC_Severity, DTC_Data);
}

void CAN_Msg_VCU_F7_DTC_Callback(int DTC_CODE, int DTC_Severity, int DTC_Data) {
    Log_Fatal_DTC(DTC_CODE, DTC_Severity, DTC_Data);
}

void CAN_Msg_BMU_DTC_Callback(int DTC_CODE, int DTC_Severity, int DTC_Data) {
    Log_Fatal_DTC(DTC_CODE, DTC_Severity, DTC_Data);
}

void CAN_Msg_ChargeCart_DTC_Callback(int DTC_CODE, int DTC_Severity, int DTC_Data) {
    Log_Fatal_DTC(DTC_CODE, DTC_Severity, DTC_Data);
}

DTC_History * read_DTC_History() {
    return &LatestDTCs;
}

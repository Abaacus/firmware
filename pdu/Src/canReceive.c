#include "bsp.h"
#include "pdu_can.h"
#include "pdu_dtc.h"
#include "state_machine.h"
#include "controlStateMachine.h"
#include "debug.h"
#include "boardTypes.h"
#include "canReceive.h"

DTC_History LatestDTCs = { .tail = 0, .total = 0 };

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

void Log_DTC(int DTC_CODE, int DTC_Severity, int DTC_Data) {
    if (DTC_Severity == DTC_Severity_FATAL 
        || DTC_Severity == DTC_Severity_CRITICAL) {
    
        DTC_Received newDTC = { .code = DTC_CODE, 
                                .severity = DTC_Severity, 
                                .data = DTC_Data };

        LatestDTCs.dtcs[LatestDTCs.tail] = newDTC;

        LatestDTCs.tail = (LatestDTCs.tail + 1) % DTC_HISTORY_LENGTH;

        if (LatestDTCs.total < DTC_HISTORY_LENGTH) {
            LatestDTCs.total++;
        }
    }    
}

void CAN_Msg_DCU_DTC_Callback(int DTC_CODE, int DTC_Severity, int DTC_Data) {
    Log_DTC(DTC_CODE, DTC_Severity, DTC_Data);
}

void CAN_Msg_VCU_F7_DTC_Callback(int DTC_CODE, int DTC_Severity, int DTC_Data) {
    Log_DTC(DTC_CODE, DTC_Severity, DTC_Data);
}

void CAN_Msg_BMU_DTC_Callback(int DTC_CODE, int DTC_Severity, int DTC_Data) {
    Log_DTC(DTC_CODE, DTC_Severity, DTC_Data);
}

void CAN_Msg_ChargeCart_DTC_Callback(int DTC_CODE, int DTC_Severity, int DTC_Data) {
    Log_DTC(DTC_CODE, DTC_Severity, DTC_Data);
}
#include "bsp.h"
#include "pdu_can.h"
#include "pdu_dtc.h"
#include "state_machine.h"
#include "controlStateMachine.h"
#include "debug.h"
#include "boardTypes.h"
#include "canReceive.h"
#include "motorCooling.h"

void CAN_Msg_UartOverCanConfig_Callback() {
    isUartOverCanEnabled = UartOverCanConfigSignal & 0x4;
}

void CAN_Msg_VCU_EM_Power_State_Request_Callback() {
    if (EM_Power_State_Request == EM_Power_State_Request_On) {
        fsmSendEventISR(&motorFsmHandle, MTR_EV_EM_ENABLE);
    } else {
        fsmSendEventISR(&motorFsmHandle, MTR_EV_EM_DISABLE);
        sendCoolingEvent(COOLING_EVENT_DISABLE);
    }
}

void DTC_Fatal_Callback(BoardIDs board) {
    DEBUG_PRINT_ISR("DTC Receieved from board %lu \n", board);
    fsmSendEventUrgentISR(&mainFsmHandle, MN_EV_HV_FatalFailure);
    sendCoolingEvent(COOLING_EVENT_FATAL_DTC);
}

void CAN_Msg_TempMotorRight_Callback() {
    tempMotorRightSum += TempMotorRight;
    numMotorTempSamplesRight++;
}

void CAN_Msg_TempMotorLeft_Callback() {
    tempMotorLeftSum += TempMotorLeft;
    numMotorTempSamplesLeft++;
}
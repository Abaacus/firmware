/**
  *****************************************************************************
  * @file    canReceive.c
  * @author  Richard Matthews
  * @brief   Module containing callback functions for receiving CAN messages
  * @details Receives DCU HV toggle button press, charge cart messages, and
  * DTCs
  *****************************************************************************
  */

#include "canReceive.h"
#include "canReceiveCommon.h"

#include "userCan.h"
#include "bsp.h"
#include "debug.h"
#include "boardTypes.h"

#include "controlStateMachine.h"

#include "bmu_can.h"

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"


void CAN_Msg_DCU_buttonEvents_Callback()
{
	DEBUG_PRINT_ISR("DCU Button events\n");
    if (ButtonHVEnabled) {
		DEBUG_PRINT_ISR("HV Toggle button event\n");
        fsmSendEventISR(&fsmHandle, EV_HV_Toggle);
    }
}

void DTC_Fatal_Callback(BoardIDs board)
{
	DEBUG_PRINT_ISR("DTC Receieved from board %lu \n", board);
    fsmSendEventUrgentISR(&fsmHandle, EV_HV_Fault);
}

uint32_t lastChargeCartHeartbeat = 0;
bool sentChargeStartEvent = false;

void CAN_Msg_ChargeCart_Heartbeat_Callback()
{
    if (!sentChargeStartEvent) {
        if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
            fsmSendEventISR(&fsmHandle, EV_Enter_Charge_Mode);
            sentChargeStartEvent = true;
        }
    }
    lastChargeCartHeartbeat = xTaskGetTickCount();
}

void CAN_Msg_ChargeCart_ButtonEvents_Callback()
{
    if (ButtonChargeStart) {
        fsmSendEventISR(&fsmHandle, EV_Charge_Start);
    } 
    if (ButtonChargeStop) {
        fsmSendEventISR(&fsmHandle, EV_Notification_Stop);
    }
    if (ButtonHVEnabled) {
        fsmSendEventISR(&fsmHandle, EV_HV_Toggle);
    }
}

void CAN_Msg_UartOverCanConfig_Callback()
{
	isUartOverCanEnabled = UartOverCanConfigSignal & 0x2;	
}

void CAN_Msg_PDU_DTC_Callback(int16_t DTC_CODE, uint8_t DTC_Severity, uint64_t DTC_Data) {
    CAN_Receive_Log_DTC(DTC_CODE, DTC_Severity, DTC_Data);
}

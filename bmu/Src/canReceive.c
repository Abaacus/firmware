/**
  *****************************************************************************
  * @file    canReceive.c
  * @author  Richard Matthews
  * @brief   Module containing callback functions for receiving CAN messages
  * @details Receives DCU HV toggle button press, charge cart messages, and
  * DTCs
  *****************************************************************************
  */

#include "batteries.h"
#include "canReceive.h"

#include "userCan.h"
#include "bsp.h"
#include "debug.h"
#include "boardTypes.h"

#include "controlStateMachine.h"
#include "state_of_charge.h"

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

void CAN_Msg_SetVariableBMU_Callback()
{
    switch (VariableEnumBMU)
    {
    case maxChargeCurrent_name:
        if (setMaxChargeCurrent(VariableValueBMU) == HAL_ERROR)
        {
            ERROR_PRINT("Failed to update max charge current\n");
        }
        break;
    
    case adjustedCellI_name:
        if (setadjustedCellIR(VariableValueBMU) == HAL_ERROR)
        {
            ERROR_PRINT("Failed to update adjusted cell IR\n");
        }
        break;

    case maxChargeVoltage_name:
        if (setMaxChargeVoltage(VariableValueBMU) == HAL_ERROR)
        {
            ERROR_PRINT("Failed to update max charge voltage\n");
        }
        break;

    case stateBusHVSendPeriod_name:
    /* TODO: do we need check anything for send period ?*/
        cliSetStateBusHVSendPeriod(VariableValueBMU);
        break;

    case capacityStartup_name:
        if (setCapacityStartup(VariableValueBMU) != HAL_OK)
        {
            ERROR_PRINT("Failed to update startup capacity value\n");
        }
        break;

    case iBusIntegrated_name:
        if (cliSetIBus(VariableValueBMU) != HAL_OK)
        {
            ERROR_PRINT("Failed to update startup capacity value\n");
        }
        break;

    default:
        break;
    }
}

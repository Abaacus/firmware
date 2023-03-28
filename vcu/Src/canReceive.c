#include "canReceive.h"

#include "userCan.h"
#include "bsp.h"
#include "debug.h"

#include "drive_by_wire.h"

#include "vcu_F7_can.h"
#include "vcu_F7_dtc.h"

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#include "endurance_mode.h"
#include "traction_control.h"
#include "motorController.h"

/*
 * External Board Statuses:
 * Variables for keeping track of external board statuses that get updated by
 * Can messages
 */
volatile bool motorControllersStatus = false;
uint32_t lastBrakeValReceiveTimeTicks = 0;
/*
 * Functions to get external board status
 */
bool getHvEnableState()
{
    return HV_Power_State == HV_Power_State_On;
}

bool getMotorControllersStatus()
{
    return motorControllersStatus;
}

extern osThreadId driveByWireHandle;

void CAN_Msg_DCU_buttonEvents_Callback()
{
    DEBUG_PRINT_ISR("Received DCU button Event\n");
    if (ButtonEMEnabled) {
        fsmSendEventISR(&fsmHandle, EV_EM_Toggle);
    }
    else if(ButtonEnduranceToggleEnabled) 
    {
		toggle_endurance_mode();
	}
	else if(ButtonEnduranceLapEnabled)
	{
		trigger_lap();
	}
	else if(ButtonTCEnabled)
	{
		toggle_TC();
	}
}

void CAN_Msg_PDU_ChannelStatus_Callback()
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    if (!motorControllersStatus && StatusPowerMCLeft == StatusPowerMCLeft_CHANNEL_ON &&
        StatusPowerMCRight == StatusPowerMCRight_CHANNEL_ON) {
        xTaskNotifyFromISR( driveByWireHandle,
                            (1<<NTFY_MCs_ON),
                            eSetBits,
                            &xHigherPriorityTaskWoken );
        motorControllersStatus = true;
    } else if (motorControllersStatus) {
        // Only send a notification if MCs turned off if MCs were already ON
        xTaskNotifyFromISR( driveByWireHandle,
                            (1<<NTFY_MCs_OFF),
                            eSetBits,
                            &xHigherPriorityTaskWoken );
        motorControllersStatus = false;
    }

    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void DTC_Fatal_Callback(BoardIDs board)
{
    DEBUG_PRINT_ISR("DTC Receieved from board %lu \n", board);
    fsmSendEventUrgentISR(&fsmHandle, EV_Fatal);
}

void CAN_Msg_BMU_HV_Power_State_Callback() {
    DEBUG_PRINT_ISR("Receive hv power state\n");
    if (HV_Power_State != HV_Power_State_On) {
        fsmSendEventISR(&fsmHandle, EV_Hv_Disable);
    }
}

void CAN_Msg_BMU_BrakePedalValue_Callback()
{
    lastBrakeValReceiveTimeTicks = xTaskGetTickCount();
}

void CAN_Msg_BMU_DTC_Callback(int DTC_CODE, int DTC_Severity, int DTC_Data) {
    switch (DTC_CODE) {
        case WARNING_CONTACTOR_OPEN_IMPENDING:
            fsmSendEventISR(&fsmHandle, EV_Hv_Disable);
            break;
        default:
            // Do nothing, other events handled by fatal callback
            break;
    }
}

void CAN_Msg_TempInverterLeft_Callback() {
    static uint32_t lastLeftInverterDTC = 0;
	if (pdMS_TO_TICKS(xTaskGetTickCountFromISR() - lastLeftInverterDTC) <= 2500)
    {
		return;
	}
	if (StateInverterLeft == 0x25)
    {
		sendDTC_WARNING_MOTOR_CONTROLLERS_FAULT_OFF(1);
	    lastLeftInverterDTC = xTaskGetTickCountFromISR();
	}
}

void CAN_Msg_TempInverterRight_Callback() {
    static uint32_t lastRightInverterDTC = 0;
	if (pdMS_TO_TICKS(xTaskGetTickCountFromISR() - lastRightInverterDTC) <= 2500)
    {
		return;
	}
	if (StateInverterRight == 0x25)
    {
		sendDTC_WARNING_MOTOR_CONTROLLERS_FAULT_OFF(0);
	    lastRightInverterDTC = xTaskGetTickCountFromISR();
	}
}


void CAN_Msg_UartOverCanConfig_Callback()
{
    isUartOverCanEnabled = UartOverCanConfigSignal & 0x1;
}

void CAN_Msg_PDU_DTC_Callback(int DTC_CODE, int DTC_Severity, int DTC_Data) {
    switch (DTC_CODE)
    {
        case ERROR_DCDC_Shutoff:
            //The DCDC unexpectedly stopped working. The PDU turned off cooling and the motors, now disable EM
            if (fsmGetState(&fsmHandle) == STATE_EM_Enable)
            {
                fsmSendEventISR(&fsmHandle, EV_EM_Toggle);
            }
            break;
        default:
            // Do nothing, other events handled by fatal callback
            break;
    }
}

void CAN_Msg_SetVariableVCU_Callback() 
{
    switch (VariableEnumVCU)
    {
        case VCU_CAN_CONFIGURED_INITIAL_SOC:
            set_initial_soc(VariableValueVCU);
            break;
        case VCU_CAN_CONFIGURED_NUM_LAPS:
            set_num_laps(VariableValueVCU);
            break;
        case VCU_CAN_CONFIGURED_NUM_LAPS_TO_COMPLETE:
            set_num_laps_complete(VariableValueVCU);
            break;
        case VCU_CAN_CONFIGURED_IN_ENDURANCE_MODE:
            set_in_endurance_mode(VariableValueVCU);
            break;
        case VCU_CAN_CONFIGURED_EM_KP:
            set_em_kP(VariableValueVCU);
            break;
        case VCU_CAN_CONFIGURED_EM_KI:
            set_em_kI(VariableValueVCU);
            break;
        case VCU_CAN_CONFIGURED_TC_ON:
            set_TC(VariableValueVCU);
            break;
        case VCU_CAN_CONFIGURED_TC_KP:
            set_tc_kP(VariableValueVCU);
            break;
        case VCU_CAN_CONFIGURED_ERROR_FLOOR:
            set_error_floor(VariableValueVCU);
            break;
        case VCU_CAN_CONFIGURED_ADJUSTMENT_TORQUE_FLOOR:
            set_adjustment_torque_floor(VariableValueVCU);
            break;
        case VCU_CAN_CONFIGURED_TV_DEAD_ZONE_END_RIGHT:
            set_tv_deadzone_end_right(VariableValueVCU);
            break;
        case VCU_CAN_CONFIGURED_TV_DEAD_ZONE_END_LEFT:
            set_tv_deadzone_end_left(VariableValueVCU);
            break;
        case VCU_CAN_CONFIGURED_TORQUE_VECTOR_FACTOR:
            set_torque_vector_factor(VariableValueVCU);
            break;
        case VCU_CAN_CONFIGURED_MAX_TORQUE_DEMAND_DEFAULT:
            set_max_torque_demand_default(VariableValueVCU);
            break;
        default:
            DEBUG_PRINT("Default case. Nothing get processed in CAN_Msg_SetVariableVCU_Callback function ");
            break;
        }
}
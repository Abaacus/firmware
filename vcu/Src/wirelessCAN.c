#include "vcu_F7_can.h"
#include "canReceive.h"
#include "debug.h"
#include "motorController.h"
#include "traction_control.h"
#include "endurance_mode.h"

#define WIRELESS_CAN_TASK_PERIOD 250

void CAN_Msg_WiCAN_SetBMU_Callback();
static void WiCANVarsPublish(void);

void wirelessCANTask(void const * argument)
{
    DEBUG_PRINT("Starting up!!\n");
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (1)
    {
        WiCANVarsPublish();
        vTaskDelayUntil(&xLastWakeTime, WIRELESS_CAN_TASK_PERIOD);
    }
}

void CAN_Msg_WiCAN_SetVCU_Callback() 
{
    switch (WiCANSetVCUEnum)
    {
        case VCU_CAN_CONFIGURED_INITIAL_SOC:
        {
            float newInitialSOC = WiCANSetVCUValue * WIRELESS_CAN_FLOAT_SCALAR;
            set_initial_soc(newInitialSOC);
            break;
        }
        case VCU_CAN_CONFIGURED_NUM_LAPS:
            set_num_laps_completed((uint32_t)WiCANSetVCUValue);
            break;

        case VCU_CAN_CONFIGURED_NUM_LAPS_TO_COMPLETE:
        {
            float newNumLapsToComplete = WiCANSetVCUValue * WIRELESS_CAN_FLOAT_SCALAR;
            set_laps_to_complete(newNumLapsToComplete);
            break;
        }

        case VCU_CAN_CONFIGURED_IN_ENDURANCE_MODE:
            set_in_endurance_mode((bool)WiCANSetVCUValue);
            break;

        case VCU_CAN_CONFIGURED_EM_KP:
        {
            float newEMKp = WiCANSetVCUValue * WIRELESS_CAN_FLOAT_SCALAR;
            set_em_kP(newEMKp);
            break;
        }

        case VCU_CAN_CONFIGURED_EM_KI:
        {
            float newEMKi = WiCANSetVCUValue * WIRELESS_CAN_FLOAT_SCALAR;
            set_em_kI(newEMKi);
            break;
        }

        case VCU_CAN_CONFIGURED_TC_ON:
            set_TC_enabled((bool)WiCANSetVCUValue);
            break;

        case VCU_CAN_CONFIGURED_TC_KP:
        {
            float newEMKp = WiCANSetVCUValue * WIRELESS_CAN_FLOAT_SCALAR;
            set_tc_kP(newEMKp);
            break;
        }

        case VCU_CAN_CONFIGURED_TC_ERROR_FLOOR_RAD_S:
        {
            float newTCErrorFloorRADS = WiCANSetVCUValue * WIRELESS_CAN_FLOAT_SCALAR;
            set_tc_error_floor_rad_s(newTCErrorFloorRADS);
            break;
        }
        
        case VCU_CAN_CONFIGURED_TC_MIN_PERCENT_ERROR:
        {
            float newTCMinPercentError = WiCANSetVCUValue * WIRELESS_CAN_FLOAT_SCALAR;
            set_tc_min_percent_error(newTCMinPercentError);
            break;
        }

        case VCU_CAN_CONFIGURED_TC_TORQUE_MAX_FLOOR:
        {
            float newTCTorqueMaxFloor = WiCANSetVCUValue * WIRELESS_CAN_FLOAT_SCALAR;
            set_tc_torque_max_floor(newTCTorqueMaxFloor);
            break;
        }

        case VCU_CAN_CONFIGURED_TV_DEAD_ZONE_END_RIGHT:
        {
            float newTVDeadZoneEndRight = WiCANSetVCUValue * WIRELESS_CAN_FLOAT_SCALAR;
            set_tv_deadzone_end_right(newTVDeadZoneEndRight);
            break;
        }

        case VCU_CAN_CONFIGURED_TV_DEAD_ZONE_END_LEFT:
        {
            float newTVDeadZoneEndLeft = WiCANSetVCUValue * WIRELESS_CAN_FLOAT_SCALAR;
            set_tv_deadzone_end_left(newTVDeadZoneEndLeft);
            break;
        }

        case VCU_CAN_CONFIGURED_TORQUE_VECTOR_FACTOR:
        {
            float newTorqueVectorFactor = WiCANSetVCUValue * WIRELESS_CAN_FLOAT_SCALAR;
            set_torque_vector_factor(newTorqueVectorFactor);
            break;
        }

        case VCU_CAN_CONFIGURED_MAX_TORQUE_DEMAND:
        {
            float newMaxTorqueDemand = WiCANSetVCUValue * WIRELESS_CAN_FLOAT_SCALAR;
            set_max_torque_demand(newMaxTorqueDemand);
            break;
        }

        default:
            DEBUG_PRINT("VCU variable %lu not supported\r\n", (uint32_t) WiCANSetVCUEnum);
            break;
        }
}

static void WiCANVarsPublish(void)
{
    static VCU_CAN_CONFIGURED_E vcuCANConfiguredIndex = VCU_CAN_CONFIGURED_INITIAL_SOC;
    WiCANFeedbackVCUEnum = vcuCANConfiguredIndex;
    switch (vcuCANConfiguredIndex)
    {
        case VCU_CAN_CONFIGURED_INITIAL_SOC:
            WiCANFeedbackVCUValue = get_initial_soc() / WIRELESS_CAN_FLOAT_SCALAR;
            sendCAN_WiCAN_FeedbackVCU();
            break;
        
        case VCU_CAN_CONFIGURED_NUM_LAPS:
            WiCANFeedbackVCUValue = get_num_laps();
            sendCAN_WiCAN_FeedbackVCU();
            break;

        case VCU_CAN_CONFIGURED_NUM_LAPS_TO_COMPLETE:
            WiCANFeedbackVCUValue = get_num_laps_complete();
            sendCAN_WiCAN_FeedbackVCU();
            break;

        case VCU_CAN_CONFIGURED_IN_ENDURANCE_MODE:
            WiCANFeedbackVCUValue = get_in_endurance_mode();
            sendCAN_WiCAN_FeedbackVCU();
            break;

        case VCU_CAN_CONFIGURED_EM_KP:
            WiCANFeedbackVCUValue = get_em_kP() / WIRELESS_CAN_FLOAT_SCALAR;
            sendCAN_WiCAN_FeedbackVCU();
            break;

        case VCU_CAN_CONFIGURED_EM_KI:
            WiCANFeedbackVCUValue = get_em_kI() / WIRELESS_CAN_FLOAT_SCALAR;
            sendCAN_WiCAN_FeedbackVCU();
            break;

        case VCU_CAN_CONFIGURED_TC_ON:
            WiCANFeedbackVCUValue = get_tc();
            sendCAN_WiCAN_FeedbackVCU();
            break;

        case VCU_CAN_CONFIGURED_TC_KP:
            WiCANFeedbackVCUValue = get_tc_kP() / WIRELESS_CAN_FLOAT_SCALAR;
            sendCAN_WiCAN_FeedbackVCU();
            break;

        case VCU_CAN_CONFIGURED_TC_ERROR_FLOOR_RAD_S:
            WiCANFeedbackVCUValue = get_tc_error_floor_rad_s() / WIRELESS_CAN_FLOAT_SCALAR;
            sendCAN_WiCAN_FeedbackVCU();
            break;

        case VCU_CAN_CONFIGURED_TC_MIN_PERCENT_ERROR:
            WiCANFeedbackVCUValue = get_tc_min_percent_error() / WIRELESS_CAN_FLOAT_SCALAR;
            sendCAN_WiCAN_FeedbackVCU();
            break;

        case VCU_CAN_CONFIGURED_TC_TORQUE_MAX_FLOOR:
            WiCANFeedbackVCUValue = get_tc_torque_max_floor() / WIRELESS_CAN_FLOAT_SCALAR;
            sendCAN_WiCAN_FeedbackVCU();
            break;

        case VCU_CAN_CONFIGURED_TV_DEAD_ZONE_END_RIGHT:
            WiCANFeedbackVCUValue = get_tv_deadzone_end_right() / WIRELESS_CAN_FLOAT_SCALAR;
            sendCAN_WiCAN_FeedbackVCU();
            break;

        case VCU_CAN_CONFIGURED_TV_DEAD_ZONE_END_LEFT:
            WiCANFeedbackVCUValue = get_tv_deadzone_end_left() / WIRELESS_CAN_FLOAT_SCALAR;
            sendCAN_WiCAN_FeedbackVCU();
            break;

        case VCU_CAN_CONFIGURED_TORQUE_VECTOR_FACTOR:
            WiCANFeedbackVCUValue = get_torque_vector_factor() / WIRELESS_CAN_FLOAT_SCALAR;
            sendCAN_WiCAN_FeedbackVCU();
            break;

        case VCU_CAN_CONFIGURED_MAX_TORQUE_DEMAND:
            WiCANFeedbackVCUValue = get_max_torque_demand() / WIRELESS_CAN_FLOAT_SCALAR;
            sendCAN_WiCAN_FeedbackVCU();
            break;

        case VCU_CAN_CONFIGURED_COUNT:
        default:
            break;

    }
    vcuCANConfiguredIndex = (vcuCANConfiguredIndex + 1) % VCU_CAN_CONFIGURED_COUNT;
}

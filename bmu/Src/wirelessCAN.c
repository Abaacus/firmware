#include "bmu_can.h"
#include "debug.h"
#include "state_of_charge.h"
#include "canReceive.h"
#include "bmu_can.h"
#include "batteries.h"

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

void CAN_Msg_WiCAN_SetBMU_Callback()
{
    switch (WiCANSetBMUEnum)
    {
        case BMU_CAN_CONFIGURED_MAX_CHARGE_CURRENT:
            setMaxChargeCurrent(WiCANSetBMUValue);
            break;
        
        case BMU_CAN_CONFIGURED_SERIES_CELL_IR:
            setSeriesCellIR(WiCANSetBMUValue);
            break;

        case BMU_CAN_CONFIGURED_STATE_BUS_HV_SEND_PERIOD:
            setStateBusHVSendPeriod(WiCANSetBMUValue);
            break;

        case BMU_CAN_CONFIGURED_CAPACITY_STARTUP:
            setCapacityStartup(WiCANSetBMUValue);
            break;

        case BMU_CAN_CONFIGURED_IBUS_INTEGRATED:
        {
            float newIbusIntegrated = WiCANSetBMUValue * WIRELESS_CAN_FLOAT_SCALAR;
            setIBusIntegrated(newIbusIntegrated);
            break;
        }

        default:
            DEBUG_PRINT("BMU variable %lu not supported\r\n", (uint32_t)WiCANSetBMUEnum);
            break;
        }
}

static void WiCANVarsPublish(void)
{
    static BMU_CAN_CONFIGURED_E bmuCANConfiguredIndex = BMU_CAN_CONFIGURED_MAX_CHARGE_CURRENT;
    WiCANFeedbackBMUEnum = bmuCANConfiguredIndex;
    switch (bmuCANConfiguredIndex)
    {
        case BMU_CAN_CONFIGURED_MAX_CHARGE_CURRENT:
            WiCANFeedbackBMUValue = getMaxChargeCurrent() / WIRELESS_CAN_FLOAT_SCALAR;
            sendCAN_WiCAN_FeedbackBMU();
            break;
        case BMU_CAN_CONFIGURED_SERIES_CELL_IR:
            WiCANFeedbackBMUValue = getSeriesCellIR() / WIRELESS_CAN_FLOAT_SCALAR;
            sendCAN_WiCAN_FeedbackBMU();
            break;
        case BMU_CAN_CONFIGURED_STATE_BUS_HV_SEND_PERIOD:
            WiCANFeedbackBMUValue = getStateBusHVSendPeriod();
            sendCAN_WiCAN_FeedbackBMU();
            break;
        case BMU_CAN_CONFIGURED_CAPACITY_STARTUP:
            WiCANFeedbackBMUValue = getCapacityStartup() / WIRELESS_CAN_FLOAT_SCALAR;
            sendCAN_WiCAN_FeedbackBMU();
            break;
        case BMU_CAN_CONFIGURED_IBUS_INTEGRATED:
            WiCANFeedbackBMUValue = getIBusIntegrated() / WIRELESS_CAN_FLOAT_SCALAR;
            sendCAN_WiCAN_FeedbackBMU();
            break;

        case BMU_CAN_CONFIGURED_COUNT:
        default:
            break;
    }
    bmuCANConfiguredIndex = (bmuCANConfiguredIndex + 1) % BMU_CAN_CONFIGURED_COUNT;
}

#include "brakeLight.h"
#include "bsp.h"
#include "FreeRTOS.h"
#include "task.h"
#include "pdu_can.h"
#include "debug.h"

#define BRAKE_TASK_PERIOD_MS 300

bool isBrakePressed()
{
    return BrakePercent > brakelightOnThreshold;
}

void CAN_Msg_VCU_Data_Callback()
{
    if (isBrakePressed()) {
        BRAKE_LIGHT_ENABLE;
    } else {
        BRAKE_LIGHT_DISABLE;
    }
}

void setBrakeLightOnThresholdPercent(uint8_t percent)
{
    if (percent > 100) {
        percent = 100;
    }
    if (percent < 0) {
        percent = 0;
    }
    
    brakelightOnThreshold = percent;
}


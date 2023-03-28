#include "brakeLight.h"
#include "bsp.h"
#include "FreeRTOS.h"
#include "task.h"
#include "pdu_can.h"
#include "debug.h"

#define BRAKE_LIGHT_ON_THRESHOLD_DEFAULT 15
#define BRAKE_TASK_PERIOD_MS 300

static float brake_light_on_threshold = BRAKE_LIGHT_ON_THRESHOLD_DEFAULT;

bool isBrakePressed(uint32_t brakePercent)
{
    return (BrakePercent > brake_light_on_threshold);
}

void CAN_Msg_VCU_Data_Callback()
{
    if (isBrakePressed(BrakePercent)) {
        BRAKE_LIGHT_ENABLE;
    } else {
        BRAKE_LIGHT_DISABLE;
    }
}

void setBrakeLightOnThreshold(float brake_light_on_threshold_value)
{
    brake_light_on_threshold = brake_light_on_threshold_value;
}
